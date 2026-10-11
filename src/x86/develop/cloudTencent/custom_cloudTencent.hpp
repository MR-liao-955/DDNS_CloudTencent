
#include <iostream>
#include <string.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <vector>

#include <curl/curl.h>
#include "config.h"

#pragma once
using namespace std;

typedef struct{
    uint64_t id;
    string value;
    string type;    // A AAAA
}record_list_t;

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

    std::vector<record_list_t> record_ipv4;
    std::vector<record_list_t> record_ipv6;

    uint64_t domain_id;
    string domain_grade;

    int dnspod_record_TXT_modify(ip_type_t ip_type, string ip_addr);
    int dnspod_record_modify(ip_type_t ip_type, string ip_addr);
    bool dnspod_domain_list();
    bool dnspod_record_list();
    bool dnspod_record_line_list();
    bool dnspod_record_list_filter();
    bool dnspod_record_delete(string domain, uint32_t record_id);   //
    void record_delete_remain_one(void);

private:
    /*  https 连接时认证使用  */
    string request_authorization;
    string security_id;
    string security_key;
    string request_version;
    string request_time_utc;
    uint64_t request_timestamp;
    uint32_t request_nonce;
    string request_content_type;

    // 默认构造函数
    cloudTencent_dnspod()
    {
        domain_id = 0;
        request_version = TENCENT_REQUEST_DEF_VERSION;
        request_content_type = TENCENT_REQUEST_DEF_CONTENT_TYPE;
        file_security_config_read();
    };

    cloudTencent_dnspod(const cloudTencent_dnspod &) = delete;
    cloudTencent_dnspod &operator=(const cloudTencent_dnspod &) = delete;

    void file_security_config_read(void);
    void generate_http_authorization(string method, string payload);
    void generate_http_header(curl_slist **headers, string action);

};
