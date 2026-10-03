
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
    char format;
    uint32_t domain_id;
    uint32_t record_id;
    string domain_grade;

    struct SubDomainInfo
    {
        uint32_t id;         // 子域名记录ID（来自 record.id）
        string name;         // 子域名（来自 record.sub_domain）
    };

    // 解析后存储的域名信息
    string domain_name;              // 域名（来自 domain.domain）
    uint32_t parsed_domain_id;       // 域名ID（来自 domain.id，与输入 domain_id 可能不同）

    // 子域名记录数组
    SubDomainInfo sub_domain_list[5];
    int sub_domain_count;            // 实际解析到的子域名数量
    int set_ipv4_dns_record(string sub_domain);
    int set_ipv6_dns_record(string sub_domain);

    bool dnspod_domain_list();

    // 默认构造函数
    cloudTencent_dnspod()
    {
        domain_id = 0;
        record_id = 0;
        format = 0;
        parsed_domain_id = 0;
        sub_domain_count = 0;

        domain_name = _MACRO_TOP_DOMAIN_URL;
        security_key_path = TENCENT_SECURITYKEY_PATH;
        request_version = TENCENT_REQUEST_DEF_VERSION;
        request_action = TENCENT_REQUEST_DEF_ACTION;
        request_content_type = TENCENT_REQUEST_DEF_CONTENT_TYPE;
    };

private:

// 请求方法 + 请求主机 +请求路径 + ? + 请求字符串。
// $secretKey = '********************************';
// $srcStr = 'GETcvm.tencentcloudapi.com/?Action=DescribeInstances&InstanceIds.0=ins-09dx96dg&Limit=20&Nonce=11886&Offset=0&Region=ap-guangzhou&SecretId=AKID********************************&Timestamp=1465185768&Version=2017-03-12';
// $signStr = base64_encode(hash_hmac('sha1', $srcStr, $secretKey, true));
// echo $signStr;

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

    void generate_http_post_authorization(string method, string payload);

};
