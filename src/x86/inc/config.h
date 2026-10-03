#ifndef _CONFIG_H
#define _CONFIG_H


#define _MACRO_TOP_DOMAIN_URL       "dearl.top"
#define _MACRO_NAS_DOMAIN           "nas"
#define _MACRO_MOV_DOMAIN           "mov"
#define _MACRO_NAS_DOMAIN_URL           _MACRO_NAS_DOMAIN "." _MACRO_TOP_DOMAIN_URL
#define _MACRO_MOV_DOMAIN_URL           _MACRO_MOV_DOMAIN "." _MACRO_TOP_DOMAIN_URL

// #define _MACRO_SERVER_GET_WANIP     "https://ip.sb"
// #define _MACRO_SERVER_GET_WANIP     "https://myip.ipip.net/s"
#define _MACRO_SERVER_GET_WANIP     "https://ip.3322.net/"


#define TENCENT_REQUEST_URL                     "dnspod.tencentcloudapi.com"

#define TENCENT_SECURITYKEY_PATH                "/home/workspace/tencent_cloud_security_key.txt"
#define TENCENT_REQUEST_DEF_VERSION             "2021-03-23"
#define TENCENT_REQUEST_DEF_ACTION              "DescribeDomainList"
#define TENCENT_REQUEST_DEF_SECRETID            "AKIDthIiyV1UrESWRAlRAzpW5Tn7VwGWq6FE"      // 根据创建的 ID 来确定。
#define TENCENT_REQUEST_DEF_CONTENT_TYPE        "application/json"

#endif
