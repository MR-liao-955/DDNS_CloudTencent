#ifndef _CONFIG_H
#define _CONFIG_H

#include <string.h>
#include <iostream>

#define VERSION                                 "0.2.1"

#if 1
#define DEBUG_PRINT(fmt, ...)  printf("[DEBUG] [%s][%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) do {...}while(0);
#endif

#define CONFIG_PATH                             "./ddns_config.txt"
#define TENCENT_REQUEST_URL                     "dnspod.tencentcloudapi.com"
#define TENCENT_REQUEST_DEF_VERSION             "2021-03-23"
#define TENCENT_REQUEST_DEF_CONTENT_TYPE        "application/json"

/*
    仅允许修改一个子域名 的 IP
    https://ip.3322.net/        // 推荐 Ipv4用
    https://myip.ipip.net/s     // Ipv6拿不到时会使用 IPv4
    https://ip.sb
*/
typedef struct {
    std::string domain_name;
    std::string domain_subname;
    std::string domain_full;

    std::string check_ip_url_ipv4;  //https://ip.3322.net/
    std::string check_ip_url_ipv6;  //https://myip.ipip.net/s
} config_t;

extern config_t g_file_config;
#endif
