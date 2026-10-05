#ifndef _HTTP_CHECK_IP_WAN_H
#define _HTTP_CHECK_IP_WAN_H
#include "config.h"

using namespace std;

#define _RETURN_SUCC 0
#define _RETURN_FAIL -1


void check_demo_http_check();

class Ipaddress // 暂不区分 IPv4 和 v6
{
public:
    typedef enum {
        CLASS_IPV6 = 0,
        CLASS_IPV4

    }ip_type_t;

    ip_type_t class_type_ip;
    string domain;
    string ip_domain;
    string ip_wan;
    string ip_ddns_modify;
    string ip_local;

    bool valid_ipwan = false;
    // 根据子类的类型来确定用何种ip
    void check_local_ip();

    int match_wanip_with_localip();

    void print_domain() const { cout << domain << endl; }
    string get_domain() const { return domain; }
    string set_domain(string _domain) { this->domain = _domain; }
    // 默认构造函数
    Ipaddress()
    {
        domain = "Ipaddress construction function() ";
    };

    // 有参构造 构造的时候就写入域名
    // Ipaddress(string domain) : domain(domain) {};
};

class _IPv4 : public Ipaddress
{
public:
    static _IPv4 &getInstance(std::string _domain)
    {
        static _IPv4 instance(_domain);
        return instance;
    }

    static _IPv4 &getInstance()
    {
        return getInstance("");
    }



    void check_nat_wan_ip() ;
    void check_domain_ip() ;

private:
    _IPv4& operator=(const _IPv4 &) = delete;
    _IPv4(const _IPv4 &) = delete;

    _IPv4(std::string _domain) {
        (this->domain = _domain);
        class_type_ip = CLASS_IPV4;
    }
    ~_IPv4() {}
};



class _IPv6 : public Ipaddress
{
public:
    static _IPv6 & getInstance(std::string _domain)
    {
        static _IPv6 instance(_domain);
        return instance;
    }


    void check_nat_wan_ip() ;
    void check_domain_ip() ;

private:
    _IPv6 &operator=(const _IPv6 &) = delete;
    _IPv6(const _IPv6 &) = delete;

    _IPv6(std::string _domain)
    {
        this->domain = _domain;
        class_type_ip = CLASS_IPV6;
    }
};

#endif
