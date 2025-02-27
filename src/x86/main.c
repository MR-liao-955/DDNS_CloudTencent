// #include <time.h>

// #include <ctype.h>
// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <stdint.h>
// #include <openssl/sha.h>
// #include <openssl/hmac.h>
// #include "curl/curl.h"

// void get_utc_date(int64_t timestamp, char *utc, int len)
// {
//     // time_t timenow;
//     struct tm sttime;
//     sttime = *gmtime(&timestamp);
//     strftime(utc, len, "%Y-%m-%d", &sttime);
// }

// void sha256_hex(const char *str, char *result)
// {
//     char buf[3];
//     unsigned char hash[SHA256_DIGEST_LENGTH];
//     SHA256_CTX sha256;
//     SHA256_Init(&sha256);
//     SHA256_Update(&sha256, str, strlen(str));
//     SHA256_Final(hash, &sha256);
//     for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
//     {
//         snprintf(buf, sizeof(buf), "%02x", hash[i]);
//         strcat(result, buf);
//     }
// }

// void hmac_sha256(const char *key, int key_len,
//                  const char *input, int input_len,
//                  unsigned char *output, unsigned int *output_len)
// {
//     HMAC_CTX *h;
// #if OPENSSL_VERSION_NUMBER < 0x10100000L
//     HMAC_CTX hmac;
//     HMAC_CTX_init(&hmac);
//     h = &hmac;
// #else
//     h = HMAC_CTX_new();
// #endif

//     HMAC_Init_ex(h, key, key_len, EVP_sha256(), NULL);
//     HMAC_Update(h, (unsigned char *)input, input_len);
//     HMAC_Final(h, output, output_len);

// #if OPENSSL_VERSION_NUMBER < 0x10100000L
//     HMAC_CTX_cleanup(h);
// #else
//     HMAC_CTX_free(h);
// #endif
// }

// void hex_encode(const char *input, int input_len, char *output)
// {
//     static const char *const lut = "0123456789abcdef";
//     char add_out[128] = {0};
//     char temp[2] = {0};
//     for (size_t i = 0; i < input_len; ++i)
//     {
//         const unsigned char c = input[i];
//         temp[0] = lut[c >> 4];
//         strcat(add_out, temp);
//         temp[0] = lut[c & 15];
//         strcat(add_out, temp);
//     }
//     strncpy(output, add_out, 128);
// }

// void lowercase(const char *src, char *dst)
// {
//     for (int i = 0; src[i]; i++)
//     {
//         dst[i] = tolower(src[i]);
//     }
// }

// struct WriteData
// {
//     char *response;
//     size_t size;
// };

// int debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userdata)
// {
//     if (type == CURLINFO_HEADER_OUT || type == CURLINFO_DATA_OUT)
//     {
//         fwrite(data, size, 1, stdout);
//     }
//     return 0;
// }

// size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
// {
//     size_t totalSize = size * nmemb;
//     struct WriteData *data = (struct WriteData *)userdata;
//     data->response = realloc(data->response, data->size + totalSize + 1);
//     if (data->response == NULL)
//     {
//         fprintf(stderr, "Failed to allocate memory.\n");
//         return 0;
//     }
//     memcpy(data->response + data->size, ptr, totalSize);
//     data->size += totalSize;
//     data->response[data->size] = '\0';
//     return totalSize;
// }

// int main()
// {
//     // 实例化一个认证对象，入参需要传入腾讯云账户 SecretId 和 SecretKey，此处还需注意密钥对的保密
//     // 代码泄露可能会导致 SecretId 和 SecretKey 泄露，并威胁账号下所有资源的安全性。以下代码示例仅供参考，建议采用更安全的方式来使用密钥，请参见：https://cloud.tencent.com/document/product/1278/85305
//     // 密钥可前往官网控制台 https://console.cloud.tencent.com/cam/capi 进行获取
//     const char *SECRET_ID = "SecretId";
//     const char *SECRET_KEY = "SecretKey";
//     const char *TOKEN = "";

//     const char *service = "dnspod";
//     const char *host = "dnspod.tencentcloudapi.com";
//     const char *region = "";
//     const char *action = "DescribeRecord";
//     const char *version = "2021-03-23";

//     int64_t timestamp = time(NULL);
//     ;
//     char date[20] = {0};
//     get_utc_date(timestamp, date, sizeof(date));

//     printf("[%s] ---- DATE: %s %s ----\n", __func__, __DATE__, __TIME__);

//     // ************* 步骤 1：拼接规范请求串 *************
//     const char *http_request_method = "POST";
//     const char *canonical_uri = "/";
//     const char *canonical_query_string = "";
//     char canonical_headers[100] = {"content-type:application/json; charset=utf-8\nhost:"};
//     strcat(canonical_headers, host);
//     strcat(canonical_headers, "\nx-tc-action:");
//     char value[100] = {0};
//     lowercase(action, value);
//     strcat(canonical_headers, value);
//     strcat(canonical_headers, "\n");
//     const char *signed_headers = "content-type;host;x-tc-action";
//     const char *payload = "{}";
//     char hashed_request_payload[100] = {0};
//     sha256_hex(payload, hashed_request_payload);

//     char canonical_request[256] = {0};
//     sprintf(canonical_request, "%s\n%s\n%s\n%s\n%s\n%s", http_request_method, canonical_uri,
//             canonical_query_string, canonical_headers, signed_headers, hashed_request_payload);

//     // ************* 步骤 2：拼接待签名字符串 *************
//     // 为了拿到 string_to_sign
//     const char *algorithm = "TC3-HMAC-SHA256";
//     char request_timestamp[16] = {0};
//     sprintf(request_timestamp, "%d", timestamp);
//     char credential_scope[64] = {0};
//     strcat(credential_scope, date); // UTC 时间追加给 credential_scope 字符串
//     sprintf(credential_scope, "%s/%s/tc3_request", date, service);
//     char hashed_canonical_request[100] = {0};
//     sha256_hex(canonical_request, hashed_canonical_request);
//     char string_to_sign[256] = {0};
//     sprintf(string_to_sign, "%s\n%s\n%s\n%s", algorithm, request_timestamp,
//             credential_scope, hashed_canonical_request);

//     // ************* 步骤 3：计算签名 *************
//     // 取得 k_hmac_sha_sign 签名
//     char k_key[64] = {0};
//     sprintf(k_key, "%s%s", "TC3", SECRET_KEY);
//     unsigned char k_date[64] = {0};
//     unsigned int output_len = 0;
//     hmac_sha256(k_key, strlen(k_key), date, strlen(date), k_date, &output_len);
//     unsigned char k_service[64] = {0};
//     hmac_sha256(k_date, output_len, service, strlen(service), k_service, &output_len);
//     unsigned char k_signing[64] = {0};
//     hmac_sha256(k_service, output_len, "tc3_request", strlen("tc3_request"), k_signing, &output_len);
//     unsigned char k_hmac_sha_sign[64] = {0};
//     hmac_sha256(k_signing, output_len, string_to_sign, strlen(string_to_sign), k_hmac_sha_sign, &output_len);

//     char signature[128] = {0};
//     hex_encode(k_hmac_sha_sign, output_len, signature);

//     // ************* 步骤 4：拼接 Authorization *************
//     char authorization[512] = {0};
//     sprintf(authorization, "%s Credential=%s/%s, SignedHeaders=%s, Signature=%s",
//             algorithm, SECRET_ID, credential_scope, signed_headers, signature);

//     char url[100] = {0};
//     sprintf(url, "https://%s", host);
//     char authorizationHeader[1024] = {0};
//     sprintf(authorizationHeader, "Authorization: %s", authorization);
//     char hostHeader[128] = {0};
//     sprintf(hostHeader, "Host: %s", host);
//     char actionHeader[128] = {0};
//     sprintf(actionHeader, "X-TC-Action: %s", action);
//     char timestampHeader[128] = {0};
//     sprintf(timestampHeader, "X-TC-Timestamp: %s", request_timestamp);
//     char versionHeader[128] = {0};
//     sprintf(versionHeader, "X-TC-Version: %s", version);
//     char regionHeader[128] = {0};
//     sprintf(regionHeader, "X-TC-Region: %s", region);
//     char tokenHeader[128] = {0};
//     sprintf(tokenHeader, "X-TC-Token: %s", TOKEN);

//     struct WriteData resData;
//     resData.response = NULL;
//     resData.size = 0;

//     // ************* 步骤 5：构造并发起请求 *************
//     CURL *curl;
//     CURLcode res;
//     curl = curl_easy_init();
//     if (curl)
//     {
//         curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
//         curl_easy_setopt(curl, CURLOPT_URL, url);
//         curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//         curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTPS);
//         struct curl_slist *headers = NULL;
//         headers = curl_slist_append(headers, authorizationHeader);
//         headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
//         headers = curl_slist_append(headers, hostHeader);
//         headers = curl_slist_append(headers, actionHeader);
//         headers = curl_slist_append(headers, timestampHeader);
//         headers = curl_slist_append(headers, versionHeader);
//         headers = curl_slist_append(headers, regionHeader);
//         headers = curl_slist_append(headers, tokenHeader);
//         curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//         const char *data = payload;
//         curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
//         // curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);
//         // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resData);
//         res = curl_easy_perform(curl);

//         if (res == CURLE_OK)
//         {
//             printf("%s\n", resData.response);
//         }
//         else
//         {
//             printf("Request failed. Error code: %d\n", res);
//         }
//         curl_slist_free_all(headers);
//     }
//     free(resData.response);
//     curl_easy_cleanup(curl);
//     curl_global_cleanup();
//     return 0;
// }

#include "test.h"
int main()
{
    test1();
    return 0;
}
