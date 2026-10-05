
#include <iostream>
#include <string.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <curl/curl.h>
#include "config.h"

#pragma once
using namespace std;

class cloudTencent_dnspod
{
public:
    static cloudTencent_dnspod &getInstance()
    {
        static cloudTencent_dnspod instance;
        return instance;
    }

    typedef enum {
        CLASS_IPV6 = 0,
        CLASS_IPV4
    }ip_type_t;

    uint32_t domain_id;
    uint32_t record_id_ipv4;
    string record_ipv4;
    uint32_t record_id_ipv6;
    string record_ipv6;
    string domain_grade;
    string domain_name;
    string domain_subname;

    // 子域名记录数组
    int dnspod_record_TXT_modify(ip_type_t ip_type, string ip_addr);

    int dnspod_record_modify(ip_type_t ip_type, string ip_addr);

    bool dnspod_domain_list();
    bool dnspod_record_list();
    bool dnspod_record_line_list();
    bool dnspod_record_list_filter();

private:
    /*  https 连接时认证使用  */
    string request_authorization;
    string security_id;
    string security_key;
    string security_key_path;
    string request_action;
    string request_version;
    string request_time_utc;
    uint64_t request_timestamp;
    uint32_t request_nonce;
    string request_content_type;

    // 默认构造函数
    cloudTencent_dnspod()
    {
        domain_id = 0;
        domain_name = _MACRO_TOP_DOMAIN_URL;
        domain_subname = "";
        security_key_path = TENCENT_SECURITYKEY_PATH;
        request_version = TENCENT_REQUEST_DEF_VERSION;
        request_action = TENCENT_REQUEST_DEF_ACTION;
        request_content_type = TENCENT_REQUEST_DEF_CONTENT_TYPE;
    };

    cloudTencent_dnspod(const cloudTencent_dnspod &) = delete;
    cloudTencent_dnspod &operator=(const cloudTencent_dnspod &) = delete;

    void generate_http_authorization(string method, string payload);
    void generate_http_header(curl_slist **headers, string action);

};
