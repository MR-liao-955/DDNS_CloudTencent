#include <stdio.h>
// #include "app_api.h"
#include "config.h"

#include <time.h>

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define URL_API "https://dnspod.tencentcloudapi.com"
// 回调函数，处理响应数据
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data)
{
    strcat(data, ptr);   // 将接收到的内容追加到data中
    return size * nmemb; // 返回接收的字节数
}

int main(void)
{
    printf(" ----  build time: %s %s  ----\n", __DATE__, __TIME__);

    CURL *curl;
    CURLcode res;
    char data[10000] = ""; // 用于存储响应数据

    // 初始化libcurl库
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        // 设置请求的URL
        curl_easy_setopt(curl, CURLOPT_URL, URL_API);

        // 设置回调函数来处理返回数据
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

        // 忽略SSL验证（用于测试，实际生产中要小心）
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // 执行请求
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else
        {
            printf("Response data: \n%s\n", data); // 打印响应数据
        }

        // 清理
        curl_easy_cleanup(curl);
    }

    // 释放全局curl资源
    curl_global_cleanup();

    return 0;
}
