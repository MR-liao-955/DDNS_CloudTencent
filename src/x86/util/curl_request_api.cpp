
#include <openssl/hmac.h>
#include <openssl/evp.h>

#include "curl_request_api.hpp"

using namespace std;

string curl_request_post(const char* url, const char* post_fields, uint16_t post_fields_size, struct curl_slist *headers, curl_cb cb)
{
    CURL *curl = NULL;
    CURLcode res;
    string response_data;
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "[%s] ERROR: curl_easy_init() failed.\n", __func__);
        return NULL;
    }
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTPS);

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "[%s] ERROR: %s\n", __func__, curl_easy_strerror(res));
    } else {
        printf("[%s] Response:\n%s\n", __func__, response_data.c_str());
    }

    if (headers) curl_slist_free_all(headers);
    if (curl) curl_easy_cleanup(curl);

    return response_data;
};
