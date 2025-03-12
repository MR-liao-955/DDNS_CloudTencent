#ifndef _HTTP_CHECK_IP_WAN_H
#define _HTTP_CHECK_IP_WAN_H
#include "config.h"

using namespace std;

void check_demo_http_check();

class Ipaddress // 暂不区分 IPv4 和 v6
{
public:
    string domain;
    string ip_domain;
    string ip_nat_wan;
    string ip_local;
    bool valid_ip = false;
    // 根据子类的类型来确定用何种ip
    void check_local_ip();
    virtual void check_nat_wan_ip() = 0;
    virtual void check_domain_ip() = 0;

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
    void check_nat_wan_ip() override;
    void check_domain_ip() override;
    _IPv4(std::string _domain) { (this->domain = _domain); }
    ~_IPv4() {}
};

class _IPv6 : public Ipaddress
{
public:
    void check_nat_wan_ip() override;
    void check_domain_ip() override;
    _IPv6(std::string _domain)
    {
        this->domain = _domain;
    }

    _IPv6() {}
    ~_IPv6() {}
};

#endif
