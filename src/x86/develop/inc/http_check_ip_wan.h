#ifndef _HTTP_CHECK_IP_WAN_H
#define _HTTP_CHECK_IP_WAN_H


void check_demo_http_check();

class Ipaddress // 暂不区分 IPv4 和 v6
{
public:
    std::string domain = "123";
    std::string wan_ip;
    bool valid_ip = false;
    // 根据子类的类型来确定用何种ip
    void check_local_ip();
    virtual void check_wan_ip() = 0;

    std::string print_domain() const{ std::cout << domain << std::endl;}
    std::string get_domain() const{ return domain;}
    std::string set_domain(std::string _domain) { this->domain = _domain;}
    // 默认构造函数
    Ipaddress()
    {
        domain = "Ipaddress construction function() ";
    };
    // 有参构造 构造的时候就写入域名

    Ipaddress(std::string domain)
    {
        this->domain = domain;
        this->print_domain();
    };

};

class _IPv4:public Ipaddress
{
public:
    void check_wan_ip()override;
    _IPv4(){}
    ~ _IPv4(){}
};

class _IPv6:public Ipaddress
{
public:
    void check_wan_ip()override;
    _IPv6(std::string _domain)
    {
        this->domain = _domain;
    }
};



#endif
