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

// curl -X POST https://dnsapi.cn/Record.Info -d 'login_token=LOGIN_TOKEN&format=json&domain_id=2317346&record_id=16894439'

#define TENCENT_DNSPOD_URL          "https://dnsapi.cn/Record.Info"


#define DOMAIN_ID                   2317346
#define RECORD_ID                   644952
#define LOGIN_TOKEN                 "644952,cb861cd07897bca0d826f97a96f2b0b7"




#endif
