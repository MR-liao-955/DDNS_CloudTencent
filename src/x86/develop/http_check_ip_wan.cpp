#include <iostream>
#include <string.h>

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"
#include "http_check_ip_wan.h"

typedef struct net_card_s
{
    int num;
    char *net_card[10] = {0};
}net_card_t;

static void ip_local_get();


void _IPv6::check_wan_ip()
{
    std::cout << "hello c++, check_wan_ip ~ !,love from IPv6" << std::endl;
}

void Ipaddress::check_local_ip()
{
    ip_local_get();
    std::cout << "hello c++, check_local_ip ~ !,love from IPv6" << std::endl;
}

void _IPv4::check_wan_ip()
{
    std::cout << "hello c++, check_local_ip ~ !,love from IPv6" << std::endl;
}

void net_hardware_get()
{
}

void ip_local_get()
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        perror("gethostname");
        return;
    }

    struct addrinfo hints{}, *res, *p;
    hints.ai_family = AF_INET; // 仅获取 IPv4 地址
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, nullptr, &hints, &res) != 0)
    {
        perror("getaddrinfo");
        return;
    }

    for (p = res; p != nullptr; p = p->ai_next)
    {
        struct sockaddr_in *addr = (struct sockaddr_in *)p->ai_addr;
        std::cout << "Local IP: " << inet_ntoa(addr->sin_addr) << std::endl;
    }

    freeaddrinfo(res);
}


void ip_wan_http_connect()
{
    // curl 建立连接， 或者依附于全局socket

}

void check_demo_http_check()
{

    std::cout << "===================" << std::endl;
    ip_local_get();
    std::cout << "===================" << std::endl;
    // Ipaddress ip(_MACRO_NAS_DOMAIN_URL); // 如果 Ipaddress 父类有虚函数，必须子类实现它之后才能实例化对象
    _IPv4 *ip_v4 = new _IPv4();
    Ipaddress *ip_v5 = new _IPv4();
    Ipaddress *ip_v6 = new _IPv6("1,1,1,1");
    // ip_v4->check_local_ip();
    // ip_v4->check_wan_ip();
    std::cout << "ip_v4->Ipaddress::domain = " << ip_v4->Ipaddress::domain << std::endl;
    std::cout << "ip_v4->domain = " << ip_v4->domain << std::endl;
    std::cout << "hello c++, long time no see ~ ip_v4->domain = " << ip_v4->Ipaddress::domain << std::endl;
    ip_v5->check_wan_ip();
    ip_v6->check_local_ip();
    delete ip_v4;
    delete ip_v5;
    ip_v4->check_local_ip();
}
