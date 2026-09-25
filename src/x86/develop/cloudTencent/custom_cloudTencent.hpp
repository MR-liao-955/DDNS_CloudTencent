
#include <iostream>
#include <string.h>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <curl/curl.h>

using namespace std;
class cloudTencent_dnspod
{
public:
    char format;
    uint32_t domain_id;
    uint32_t record_id;

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

    string get_record_info();

    // 默认构造函数
    cloudTencent_dnspod()
    {
        domain_id = 0;
        record_id = 0;
        format = 0;
        parsed_domain_id = 0;
        sub_domain_count = 0;
    };

private:

    void record_info_decode(string info);


};
