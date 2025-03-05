#ifndef _HTTP_CHECK_IP_WAN_H
#define _HTTP_CHECK_IP_WAN_H


void check_demo_http_check();

class Ipaddress // 暂不区分 IPv4 和 v6
{
public:
    std::string domain;
    std::string wan_ip;
    bool valid_ip = false;
    // 根据子类的类型来确定用何种ip
    virtual void check_local_ip() = 0;
    virtual void check_wan_ip() = 0;
    std::string print_domain() const{ std::cout << domain << std::endl;}
    std::string get_domain() const{ return domain;}
    std::string set_domain(std::string _domain) { this->domain = _domain;}
    // 默认构造函数
    Ipaddress()
    {

    };
    // 有参构造 构造的时候就写入域名

    Ipaddress(std::string domain)
    {
        this->domain = domain;
        this->print_domain();
    };

    // 析构, 清理资源 or 释放对象的时候做任何操作。
    ~Ipaddress()
    {

    };

};

class _IPv4:public Ipaddress
{
public:

    void check_local_ip(){ std::cout << "check_local_ip() func ~ !" << std::endl;};
    void check_wan_ip(){ std::cout << "check_wan_ip() func ~ !" << std::endl;};
    _IPv4()
    {

    }

    _IPv4(std::string _str)
    {
        Ipaddress::domain = _str;
    }
    ~ _IPv4()
    {
    }

};



#endif
