#include <iostream>
#include <string.h>

// local ip1
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// local ip2
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include <cstdio>

#include "config.h"
#include "http_check_ip_wan.h"

using namespace std;

typedef struct net_card_s
{
    int num;
    char *net_card[10] = {0};
} net_card_t;

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

// 根据网段进行判断
int isIPv4_wan(char *ip)
{
    /*
        A类地址: 1.0.0.1        到  126.255.255.254
        B类地址: 128.0.0.1      到  191.255.255.254
        C类地址: 192.168.0.0    到  192.168.255.255
        D类地址属于保留地址 or 广播地址 224.0.0.1～239.255.255.254
        E类地址: 240.0.0.1      到  255.255.255.254
    */
    printf("_-- __");
    int segment[4] = {0};
    char strTemp[16] = {0};
    char *star = ip;
    char *end = NULL;
    printf("pointer &ip = %p \n",ip);
    printf("pointer &star = %p \n",star);

    for (size_t i = 0; i < 3; i++)
    {
        end = strchr(star + 1,'.');
        if(star == end)
            break;

            cout << "end-star = " << end-star << endl;
        memcpy(strTemp, star, end-star);
        segment[i] = strtol(strTemp, NULL, 10);
        cout << "segment[" << i << "] = " << segment[i] << endl;
        star = end;  // star 指向 end, end 重新查找

    }
    memcpy(strTemp, star, 3);
    segment[3] = strtol(strTemp, NULL, 10);
    cout << "segment[3] = " << segment[3] << endl;
    // TODO: 处理
}

// 前缀来判断
void isIPv6_wan()
{
}

// int ip_valid(char *ip)
int ip_valid(char *ip_test)
{
    // 2408:8459:3010:8ba0:85fb:7ef7:3696:5453
    // 192.168.79.1
    //  判断冒号数量确定是 ipv6
    int len = 0;
    int cnt = 0;
    int ret = 0;
    cout << " ---★---1 " << endl;
    char ip[256] = {0};
    memcpy(ip,ip_test,strlen(ip_test));

    len = strlen(ip);
    cout << " ---★---2 " << endl;
    cout << " ---★--- ip :" << ip  << endl;
    for (int i = 0; i < len; i++)
    {
        if (ip[i] == ':' || ip[i] == '.')
            cnt++;
        cout << " ---★--- i = "<< i <<" ,cnt ="<< cnt << endl;
    }
    switch (cnt)
    {
    case 3:
        // IPv4
        // 获取前几位 IP 判断是否是局域网
        if(ip == nullptr)
            std::cout << " ip == NULL ???? " << std::endl;

        ret = isIPv4_wan(ip);//!~bug
        if (1 == ret)
        {
            std::cout << " ipv4 is valid , ret == 1 " << std::endl;
        }

        break;
    case 7:
        // IPv6

    default:
        break;
    }
}

void ip_local_get2()
{
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
        {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) // check it is IP4
        {
            // is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN] = {0};
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            ip_valid(addressBuffer); // !bug
        }
        else if (ifa->ifa_addr->sa_family == AF_INET6) // check it is IP6
        {
            // is a valid IP6 Address
            tmpAddrPtr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        }
    }
    if (ifAddrStruct != NULL)
    {
        freeifaddrs(ifAddrStruct);
    }
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
    ip_local_get2();
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
