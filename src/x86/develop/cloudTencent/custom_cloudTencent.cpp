#include "custom_cloudTencent.hpp"
#include "custom_cloudTencent_response_decode.hpp"

#include <ctime>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iomanip>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "config.h"
#include "tretap_cJSON.h"

#include "curl_request_api.hpp"

using namespace std;

extern int tretap_base64_encode( unsigned char *dst, unsigned int *dlen, const unsigned char *src, unsigned int slen );

string HexEncode(const string &input) {
    static const char *const lut = "0123456789abcdef";
    size_t len = input.length();

    string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

string HmacSha256(const string &key, const string &input) {
    unsigned char hash[32];

    HMAC_CTX *h;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    HMAC_CTX hmac;
    HMAC_CTX_init(&hmac);
    h = &hmac;
#else
    h = HMAC_CTX_new();
#endif

    HMAC_Init_ex(h, &key[0], key.length(), EVP_sha256(), NULL);
    HMAC_Update(h, (unsigned char *)&input[0], input.length());
    unsigned int len = 32;
    HMAC_Final(h, hash, &len);

#if OPENSSL_VERSION_NUMBER < 0x10100000L
    HMAC_CTX_cleanup(h);
#else
    HMAC_CTX_free(h);
#endif

    std::stringstream ss;
    ss << std::setfill('0');
    for (int i = 0; i < len; i++) {
        ss << hash[i];
    }

    return (ss.str());
}

string sha256Hex(const string &str) {
    char buf[3];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::string NewString = "";
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        snprintf(buf, sizeof(buf), "%02x", hash[i]);
        NewString = NewString + buf;
    }
    return NewString;
}


// curl 写回调，用于接收服务器返回的数据
static size_t record_info_write_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    string *output = static_cast<string *>(userp);
    output->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

int cloudTencent_dnspod::set_ipv4_dns_record(string sub_domain)
{

}

int cloudTencent_dnspod::set_ipv6_dns_record(string sub_domain)
{

}


void cloudTencent_dnspod::generate_http_header(curl_slist **headers, string action)
{
    string temp;
    temp = "Authorization: " + request_authorization;
    *headers = curl_slist_append(*headers, temp.data());

    temp = "Content-Type: " + request_content_type;     //content-type:application/json; charset=utf-8\nhost:"TENCENT_REQUEST_URL"\n";
    *headers = curl_slist_append( *headers, temp.data());

    *headers = curl_slist_append(*headers, "Host: " TENCENT_REQUEST_URL);

    temp ="X-TC-Action: " + action;
    *headers = curl_slist_append( *headers, temp.data());

    temp = "X-TC-Timestamp: " +std::to_string(request_timestamp);
    *headers = curl_slist_append( *headers, temp.data());

    temp = "X-TC-Version: "+ request_version;
    *headers = curl_slist_append( *headers, temp.data());
    *headers = curl_slist_append( *headers, "X-TC-Language: zh-CN");

    *headers = curl_slist_append( *headers, "X-TC-Token: ");    //string tokenHeader = "X-TC-Token: " + TOKEN;
    printf("[%s] execute curl_request_post\n", __func__);

}

/***
 * @brief: 生成 signature 用于鉴权
 * @param: input: 请求方法 + 请求主机 +请求路径 + ? + 请求字符串。
 *          input_len: strlen(input);
 * @doc: https://cloud.tencent.com/document/api/1427/56190
 * @status: 还未验证
 */
void cloudTencent_dnspod::generate_http_authorization(string method, string payload)
{
    time_t t = time(nullptr);
    char timestamp_str[32];
    struct tm utc_time;
    // timestamp 初始化
    gmtime_r(&t, &utc_time);
    strftime(timestamp_str,sizeof(timestamp_str),"%Y-%m-%d",&utc_time);
    request_timestamp= t;
    request_time_utc = timestamp_str;

    /* 读取文件中的 keyid 和 key */
    std::stringstream file_read;
    std::ifstream file(security_key_path.c_str());
    file_read << file.rdbuf();
    std::string config = file_read.str();
    std::stringstream ss(config);
    std::string line;
    while (std::getline(ss, line)) {
        // Windows 文本文件可能存在 \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        // ';' 是配置项结束符
        size_t pos = line.find(';');
        if (pos != std::string::npos) {
            line = line.substr(0, pos);
        }

        if (line.compare(0, 4, "key=") == 0) {
            security_key = line.substr(4);
        }
        else if (line.compare(0, 6, "keyid=") == 0) {
            security_id = line.substr(6);
        }
    }

    printf("[%s] secret_key: %s\n", __func__, security_key.c_str());
    printf("[%s] secret_id : %s\n", __func__, security_id.c_str());

    // ************* 步骤 1：拼接规范请求串 *************
    string httpRequestMethod = method;
    string canonicalUri = "/";
    string canonicalQueryString = "";
    string canonicalHeaders = "content-type:application/json\nhost:"TENCENT_REQUEST_URL"\n";
    string signedHeaders = "content-type;host";
    string hashedRequestPayload = sha256Hex(payload);
    string canonicalRequest = httpRequestMethod + "\n" + canonicalUri + "\n" + canonicalQueryString + "\n"
                              + canonicalHeaders + "\n" + signedHeaders + "\n" + hashedRequestPayload;

    // ************* 步骤 2：拼接待签名字符串 *************
    string algorithm = "TC3-HMAC-SHA256";
    string RequestTimestamp = to_string(request_timestamp);
    string credentialScope = request_time_utc + "/" + "dnspod" + "/" + "tc3_request";
    string hashedCanonicalRequest = sha256Hex(canonicalRequest);
    string stringToSign = "TC3-HMAC-SHA256";
    stringToSign.append("\n")
                .append(RequestTimestamp)
                .append("\n")
                .append(credentialScope)
                .append("\n")
                .append(hashedCanonicalRequest);

    // ************* 步骤 3：计算签名 *************
    string kKey = "TC3" + security_key;
    string kDate = HmacSha256(kKey, request_time_utc.data());
    string kService = HmacSha256(kDate, "dnspod");
    string kSigning = HmacSha256(kService, "tc3_request");
    string signature = HexEncode(HmacSha256(kSigning, stringToSign));
    // ************* 步骤 4：拼接 Authorization *************
    request_authorization = algorithm + " " + "Credential=" TENCENT_REQUEST_DEF_SECRETID "/" + credentialScope + ", "
                        + "SignedHeaders=" + signedHeaders + ", " + "Signature=" + signature;

#if 0
    printf("\n========== TC3 DEBUG ==========\n");
    printf("httpRequestMethod = [%s]\n", httpRequestMethod.c_str());
    printf("canonicalUri      = [%s]\n", canonicalUri.c_str());
    printf("canonicalQuery    = [%s]\n", canonicalQueryString.c_str());
    printf("canonicalHeaders  = [%s]\n", canonicalHeaders.c_str());
    printf("signedHeaders     = [%s]\n", signedHeaders.c_str());
    printf("payload           = [%s]\n", payload.c_str());
    printf("hashedPayload     = [%s]\n", hashedRequestPayload.c_str());

    printf("\n----- canonicalRequest -----\n");
    printf("%s", canonicalRequest.c_str());
    printf("\n----- canonicalRequest END -----\n");

    printf("request_timestamp = [%ld]\n", request_timestamp);
    printf("request_time_utc  = [%s]\n", request_time_utc.c_str());
    printf("credentialScope   = [%s]\n", credentialScope.c_str());

    printf("hashedCanonicalRequest = [%s]\n",
        hashedCanonicalRequest.c_str());

    printf("\n----- stringToSign -----\n");
    printf("%s", stringToSign.c_str());
    printf("\n----- stringToSign END -----\n");

    printf("========== TC3 DEBUG END ==========\n\n");
#endif
}



/**
 * @brief: 获取域名列表
 * @哈基mi
 * @doc: https://cloud.tencent.com/document/product/1427/56172
 * @data: 2026/10/2
 * @author: DearL- Arthur
 */
bool cloudTencent_dnspod::dnspod_domain_list()
{
    struct curl_slist *headers = NULL;

    string url = "https://" TENCENT_REQUEST_URL;

    request_action = "DescribeDomainList";

    // 生成 authorization 用于请求头
    generate_http_authorization("POST","{}");

    generate_http_header(&headers, request_action);

    string response = curl_request_post(url.data(), "{}", 2, headers, (curl_cb )record_info_write_cb);

    printf("[%s] done !\n", __func__);

    return tencentcloud_decode_domain_list(response, this);
}

//DescribeRecordList
/**
 * @brief: 获取解析记录列表
 * @doc: https://cloud.tencent.com/document/product/1427/56166
 * @data: 2026/10/3
 * @author: DearL - Arthur
 *
*/
bool cloudTencent_dnspod::dnspod_record_list()
{
    string payload;
    struct curl_slist *headers = NULL;

    request_action = "DescribeRecordList";

    string url = "https://" TENCENT_REQUEST_URL;
    if  (domain_subname.empty()){

        payload = "{\"Domain\": \"" + domain_name + "\"}";
    }else{

        payload = "{\"Domain\": \"" + domain_name + "\", \"SubDomain\": \"" + domain_subname + "\"}";
    }


    // 生成 authorization 用于请求头
    generate_http_authorization("POST",payload);

    generate_http_header(&headers, request_action);

    string response = curl_request_post(url.data(), payload.data(), 2, headers, (curl_cb )record_info_write_cb);

    printf("[%s] done !\n", __func__);

    // return tencentcloud_decode_domain_list(response, this);
    return true;
}

// DescribeRecordFilterList
bool cloudTencent_dnspod::dnspod_record_list_filter()
{
    string payload;
    struct curl_slist *headers = NULL;

    request_action = "DescribeRecordFilterList";

    string url = "https://" TENCENT_REQUEST_URL;
    if  (domain_subname.empty())
        payload = "{\"Domain\": \"" + domain_name + "\"}";
    else
        payload = "{\"Domain\": \"" + domain_name + "\", \"SubDomain\": \"" + domain_subname + "\", \"IsExactSubDomain\": true}";

    generate_http_authorization("POST",payload);

    generate_http_header(&headers, request_action);

    string response = curl_request_post(url.data(), payload.data(), 2, headers, (curl_cb )record_info_write_cb);

    printf("[%s] done !\n", __func__);

    return tencentcloud_decode_record_list_filter(response, this);
}
