#include <iostream>
#include <string.h>

static void check_local_ip();
static void check_WAN_ip();

class ipaddress
{
public:
    std::string domain;
    std::string wan_ipv6;
    std::string wan_ipv4;
    bool valid_ipv4 = false;
    bool valid_ipv6 = false;
    void check_local_ip();
    void check_WAN_ip();
    // 默认构造函数
    ipaddress()
    {

    }

    ipaddress(std::string domain)
    {
        domain = domain;
    };

    ~ipaddress()
    {

    }
};


void check_local_ip()
{
    // 检查本地 ip
    std::cout << "hello c++, long time no see ~ !" << std::endl;
}


void check_WAN_ip()
{
    // 检查是否和 local IPv4 一致
}

void check_wan_ip()
{

}

