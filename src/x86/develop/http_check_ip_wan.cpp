#include <iostream>
#include <string.h>

#include "config.h"
#include "http_check_ip_wan.h"


void check_local_ip()
{
    // 检查本地 ip
    std::cout << "hello c++, long time no see ~ !" << std::endl;
}


void check_wan_ip()
{
    // 检查是否和 local IPv4 一致
}

void check_demo_http_check()
{


    // Ipaddress ip(_MACRO_NAS_DOMAIN_URL); // 如果 Ipaddress 父类有虚函数，必须子类实现它之后才能实例化对象
    _IPv4 * ip_v4 = new _IPv4("1234");
    // ip_v4->check_local_ip();
    // ip_v4->check_wan_ip();
    std::cout << "hello c++, long time no see ~ ip_v4->domain = " << ip_v4->Ipaddress::domain << std::endl;
}

