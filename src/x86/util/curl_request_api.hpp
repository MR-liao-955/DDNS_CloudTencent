
#include <curl/curl.h>

#include <iostream>
#include <string.h>

typedef  size_t (*curl_cb)(void *contents, size_t size, size_t nmemb, void *userp);



std::string curl_request_post(const char* url, const char* post_fields, uint16_t post_fields_size, struct curl_slist *headers, curl_cb cb);


