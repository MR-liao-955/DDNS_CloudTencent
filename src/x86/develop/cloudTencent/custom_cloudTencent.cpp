#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include "curl/curl.h"
#include "openssl/sha.h"

using namespace std;


string get_data(int64_t &timestamp) {
    string utcDate;
    char buff[20] = {0};
    struct tm sttime;
    sttime = *gmtime(&timestamp);
    strftime(buff, sizeof(buff), "%Y-%m-%d", &sttime);
    utcDate = string(buff);
    return utcDate;
}

string int2str(int64_t n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
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

int debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userdata) {
    if (type == CURLINFO_HEADER_OUT || type == CURLINFO_DATA_OUT) {
        std::cout.write(data, size);
    }
    return 0;
}

struct WriteData {
    std::string response;
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t totalSize = size * nmemb;
    WriteData *data = static_cast<WriteData *>(userdata);
    data->response.append(ptr, totalSize);
    return totalSize;
}

/* // 流程:
    1. 获取当前 IP 地址。(通过公网服务器) (优先判断本地 IPv4 是否是公网 IPv4)
    2. 访问腾讯云，获取 (子)域名指向的地址
    3. 判断是否需要更新 IPv6 地址

*/
int custom_cloudTencent_init() {
    // 实例化一个认证对象，入参需要传入腾讯云账户 SecretId 和 SecretKey，此处还需注意密钥对的保密
    // 代码泄露可能会导致 SecretId 和 SecretKey 泄露，并威胁账号下所有资源的安全性。以下代码示例仅供参考，建议采用更安全的方式来使用密钥，请参见：https://cloud.tencent.com/document/product/1278/85305
    // 密钥可前往官网控制台 https://console.cloud.tencent.com/cam/capi 进行获取
    string SECRET_ID = "SecretId";
    string SECRET_KEY = "SecretKey";
    string TOKEN = "";

    string service = "dnspod";
    string host = "dnspod.tencentcloudapi.com";
    string region = "";
    string action = "CreateRecord";
    string version = "2021-03-23";

    int64_t timestamp = time(NULL);
    string date = get_data(timestamp);

    // ************* 步骤 1：拼接规范请求串 *************
    string httpRequestMethod = "POST";
    string canonicalUri = "/";
    string canonicalQueryString = "";
    string canonicalHeaders = "content-type:application/json; charset=utf-8\nhost:" + host + "\n";
    string signedHeaders = "content-type;host";
    string payload = "{}";
    string hashedRequestPayload = sha256Hex(payload);
    string canonicalRequest = httpRequestMethod + "\n" + canonicalUri + "\n" + canonicalQueryString + "\n"
                              + canonicalHeaders + "\n" + signedHeaders + "\n" + hashedRequestPayload;

    // ************* 步骤 2：拼接待签名字符串 *************
    string algorithm = "TC3-HMAC-SHA256";
    string RequestTimestamp = int2str(timestamp);
    string credentialScope = date + "/" + service + "/" + "tc3_request";
    string hashedCanonicalRequest = sha256Hex(canonicalRequest);
    string stringToSign = algorithm + "\n" + RequestTimestamp + "\n" + credentialScope + "\n"
                          + hashedCanonicalRequest;

    // ************* 步骤 3：计算签名 *************
    string kKey = "TC3" + SECRET_KEY;
    string kDate = HmacSha256(kKey, date);
    string kService = HmacSha256(kDate, service);
    string kSigning = HmacSha256(kService, "tc3_request");
    string signature = HexEncode(HmacSha256(kSigning, stringToSign));
    // ************* 步骤 4：拼接 Authorization *************
    string authorization = algorithm + " " + "Credential=" + SECRET_ID + "/" + credentialScope + ", "
                           + "SignedHeaders=" + signedHeaders + ", " + "Signature=" + signature;

    string url = "https://" + host;
    string authorizationHeader = "Authorization: " + authorization;
    string hostHeader = "Host: " + host;
    string actionHeader = "X-TC-Action: " + action;
    string timestampHeader = "X-TC-Timestamp: " + RequestTimestamp;
    string versionHeader = "X-TC-Version: " + version;
    string regionHeader = "X-TC-Region: " + region;
    string tokenHeader = "X-TC-Token: " + TOKEN;

    WriteData resData;

    // ************* 步骤 5：构造并发起请求 *************
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, url.data());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTPS);
        // curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, authorizationHeader.data());
        headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
        headers = curl_slist_append(headers, hostHeader.data());
        headers = curl_slist_append(headers, actionHeader.data());
        headers = curl_slist_append(headers, timestampHeader.data());
        headers = curl_slist_append(headers, versionHeader.data());
        headers = curl_slist_append(headers, regionHeader.data());
        headers = curl_slist_append(headers, tokenHeader.data());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        const char *data = payload.data();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resData);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            std::cout << resData.response << std::endl;
        } else {
            std::cout << "Request failed. Error code: " << res << std::endl;
        }
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}
