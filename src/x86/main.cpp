#include <iostream>
#include <string.h>
#include <stdio.h>

#include "custom_cloudTencent.hpp"
#include "check_ip_wan.h"

// 用于 json 解析库使用。
extern void *ddns_mem_alloc(int size);
extern void ddns_mem_free(void **ptr);

void *ddns_mem_alloc(int size) {
    void *p=NULL;
    if(size){
        p = (void*)malloc(size);
    }
    if(p != NULL){
        memset(p, 0, size);
    }
    return p;
}

void ddns_mem_free(void **ptr){
    if(ptr != NULL && *ptr != NULL){
        free(*ptr);
        *ptr = NULL;
    }
}

int main() {
    std::cout << "hello world ! ~~~" << std::endl;

    // 检查是否有子类域名
    _IPv4 &ip_v4 = _IPv4::getInstance(_MACRO_TOP_DOMAIN_URL);
    _IPv6 &ip_v6 = _IPv6::getInstance(_MACRO_TOP_DOMAIN_URL);

    // 本地 IP 检查
    check_demo_http_check();

    // 腾讯云接口
    cloudTencent_dnspod &dnspod = cloudTencent_dnspod::getInstance();

    dnspod.dnspod_domain_list();

    dnspod.dnspod_record_list();     // 此函数暂时无作用
    // dnspod.dnspod_record_line_list();

    dnspod.dnspod_record_list_filter();

    // 如果是 本地IP 是公网 IP
    if(_RETURN_SUCC == ip_v4.match_wanip_with_localip()){
        // dnspod.dnspod_record_TXT_modify(cloudTencent_dnspod::CLASS_IPV4, ip_v4.ip_ddns_modify);
        dnspod.dnspod_record_modify(cloudTencent_dnspod::CLASS_IPV4, ip_v4.ip_ddns_modify);
    }


    if(_RETURN_SUCC == ip_v6.match_wanip_with_localip()){
        // dnspod.dnspod_record_TXT_modify(cloudTencent_dnspod::CLASS_IPV6, ip_v6.ip_ddns_modify);
        dnspod.dnspod_record_modify(cloudTencent_dnspod::CLASS_IPV6, ip_v6.ip_ddns_modify);
    }

}
