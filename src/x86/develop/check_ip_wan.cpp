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
#include "check_ip_wan.hpp"
#include "curl_request_api.hpp"
#include "tretap_cJSON.h"

// curl 获取IP
#include <curl/curl.h>

#define FUNC_JSON_CHECK(func, r) \
    do{ \
        r = func; \
        if(!r) {\
            printf("[%s: %d] fail \n", #func, __LINE__);\
            return false; \
         }\
    }while(0);



Ipaddress *ip_v4 = NULL;
Ipaddress *ip_v6 = NULL;

/***************************************************************
 * 检查本地 IP 是否和 公网 IP 一致
 *
***************************************************************/
size_t check_wan_ip_handle_cb(void* contents, size_t size, size_t nmemb, std::string* output)
{
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

/**
 * @brief: JSON 解析函数，解析服务器返回的string是否是当前公网
 *
 *
 */
static bool decode_ipwan(Ipaddress *ipaddr, string src)
{
    DEBUG_PRINT("response:[%s]\n", src.data());
    while (!src.empty() &&
        (src.back() == '\n' ||
            src.back() == '\r'))
    {
        src.pop_back();
    }
    ipaddr->ip_wan = src;
    DEBUG_PRINT("ip_wan:[%s]\n", ipaddr->ip_wan.data());
    return true;
}



void _IPv4::check_nat_wan_ip()
{
    string resp;
    struct curl_slist *headers = NULL;
    const char * url = g_file_config.check_ip_url_ipv4.c_str();

    headers = curl_slist_append(headers, ": ");
    resp = curl_request(url, "GET", NULL, 2, headers, (curl_cb )check_wan_ip_handle_cb);
    decode_ipwan(this, resp);
    DEBUG_PRINT("ipv4 checkdone !\n");
}

void _IPv6::check_nat_wan_ip()
{
    string resp;
    struct curl_slist *headers = NULL;
    const char * url = g_file_config.check_ip_url_ipv6.c_str();

    headers = curl_slist_append(headers, ": ");
    resp = curl_request(url, "GET", NULL, 2, headers, (curl_cb )check_wan_ip_handle_cb);
    decode_ipwan(this, resp);
    DEBUG_PRINT("ipv6 check done !\n");
}


/*
    @brief: 检查公网 IP 是否和 local ip 有任何一位匹配的。
*/
int Ipaddress:: match_wanip_with_localip()
{
    DEBUG_PRINT("Entry !\n");
#if 1
    DEBUG_PRINT("ip_wan:[%s] !\n", ip_wan.data());
    DEBUG_PRINT("ip_local:[%s] !\n", ip_local.data());
    if (valid_ipwan == true && ip_wan == ip_local){
        // 本机IP是公网
        ip_ddns_modify = ip_local;
        return _RETURN_SUCC;
    }
#else   // 测试用。
        ip_ddns_modify = ip_local;
        return _RETURN_SUCC;
#endif

    return _RETURN_FAIL;
}

// 根据网段进行判断
bool isIPv4_wan(char *ip)
{
    /*
        A类地址: 1.0.0.1        到  126.255.255.254
        B类地址: 128.0.0.1      到  191.255.255.254
        C类地址: 192.168.0.0    到  192.168.255.255
        D类地址属于保留地址 or 广播地址 224.0.0.1～239.255.255.254
        E类地址: 240.0.0.1      到  255.255.255.254

        A类私有地址: 10.0.0.0 - 10.255.255.255
        B类私有地址: 172.16.0.0 - 172.31.255.255
        C类私有地址: 192.168.0.0 - 192.168.255.255
        环回地址: 127.0.0.0 - 127.255.255.255
    */

    struct in_addr addr;
    inet_pton(AF_INET, ip, &addr);

    // 转换成16进制
    uint32_t ip_hex = ntohl(addr.s_addr); // ip :192.168.12.12 ==> 0xc0a80c0c
    // 检查是否在私有IP范围内
    if ((ip_hex > 0x0A000000 && ip_hex < 0x0AFFFFFF) || // 10.0.0.0 - 10.255.255.255
        (ip_hex > 0xAC100000 && ip_hex < 0xAC1FFFFF) || // 172.16.0.0 - 172.31.255.255
        (ip_hex > 0xC0A80000 && ip_hex < 0xC0A8FFFF) || // 192.168.0.0 - 192.168.255.255
        (ip_hex > 0x7F000000 && ip_hex < 0x7FFFFFFF))   // 127.0.0.0 - 127.255.255.255 (loopback)
    {
        printf(" private ip \n");
        return 0; // 私有IP，返回false
    }
    printf(" wan ip \n");
    return 1;
}

// 前缀来判断
bool isIPv6_wan(char *ipv6)
{
    // 判断IPv6是否私有
    // 2408:8459:3010:8ba0:85fb:7ef7:3696:5453
    // fe80::20c:29ff:fe13:2572

    // 内网
    /*
        2000::/3    公网
        fc00::/7    私有IP,类似于IPv4局域网
        fe80::/10	本地IP地址
        ff00::/8	组播地址
        ::1	        回环网络
        ::	        未指定
    */
   char temp[5] = {0};

    char * star = ipv6;
    char * end = NULL;
    for (int i = 0; i < 32; i++)
    {
        if (':' == ipv6[i])
        {
            end = star + i;
            break;
        }
    }

    if ((end - star) == 4)
    {
        memcpy(temp, star, end-star);
        uint16_t num = strtol(temp,NULL,16);
        return ((num >> 13) ==  0b001);
    }
    DEBUG_PRINT("[%s] is not ipv6_wan~\n", ipv6);
    return 0;
}

bool ip_valid(char *ip)
{
    bool ret = false;
    int len = strlen(ip);
    for (int i = 0; i < len; i++){
        if (ip[i] == '.'){
            ret = isIPv4_wan(ip); // 注意 该函数并没有保护。如果传入奇怪的IP 也能用。
            break;
        }
        else if (ip[i] == ':'){
            ret = isIPv6_wan(ip);
            break;
        }
    }
    return ret;
}

void Ipaddress::check_local_ip()
{

    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;
    char addressBuffer[INET6_ADDRSTRLEN] = {0};
    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        memset(addressBuffer,0,INET6_ADDRSTRLEN);
        if (!ifa->ifa_addr)
        {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET && class_type_ip == CLASS_IPV4) // check it is IP4
        {
            if (this->valid_ipwan) continue;
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            // memcpy(addressBuffer, "223.5.5.8", strlen("223.5.5.5"));        // 测试用
            DEBUG_PRINT("IPv4[%s] Address %s\n", ifa->ifa_name, addressBuffer);
            this->valid_ipwan = ip_valid(addressBuffer);
            DEBUG_PRINT("IPv4 wan ? %s\n", this->valid_ipwan ? "yes":"no");

        }
        else if (ifa->ifa_addr->sa_family == AF_INET6 && class_type_ip == CLASS_IPV6) // check it is IP6
        {
            if (this->valid_ipwan) continue;
            tmpAddrPtr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            // memcpy(addressBuffer,"2408:8256:f180:58f:22f:f3ff:fe3f:8a4", strlen("2408:8256:f180:58f:22f:f3ff:fe3f:8a4")); // 测试用
            DEBUG_PRINT("IPv6[%s] Address %s\n", ifa->ifa_name, addressBuffer);
            this->valid_ipwan = ip_valid(addressBuffer);
            DEBUG_PRINT("IPv6 wan ? %s\n", this->valid_ipwan  ? "yes":"no");
        }
        if (this->valid_ipwan)
        {
            this->ip_local = addressBuffer;  // tip: string类型这里的赋值是深拷贝，所以不会有问题。即使 addressBuffer 是局部变量。
            DEBUG_PRINT("ip_local: %s\n", this->ip_local.data());
            break;
        }

    }
    if (ifAddrStruct != NULL)
    {
        freeifaddrs(ifAddrStruct);
    }
}

void check_demo_http_check()
{
    int ret = 0;
    _IPv4 &ip_v4 = _IPv4::getInstance(g_file_config.domain_full);
    _IPv6 &ip_v6 = _IPv6::getInstance(g_file_config.domain_full);

    ip_v4.check_local_ip();
    ip_v6.check_local_ip();

    ip_v4.check_nat_wan_ip();
    ip_v6.check_nat_wan_ip();

    cout << "===================" << endl;
}
