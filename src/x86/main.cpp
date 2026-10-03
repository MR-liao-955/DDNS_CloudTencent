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

    // 本地 IP 检查
    check_demo_http_check();

    // 腾讯云接口
    cloudTencent_dnspod &dnspod = cloudTencent_dnspod::getInstance();

    dnspod.dnspod_domain_list();

    // dnspod.dnspod_record_list();

    dnspod.dnspod_record_list_filter();

    _IPv4::getInstance("dearl.top").check_nat_wan_ip();
    _IPv6::getInstance("dearl.top").check_nat_wan_ip();


}
