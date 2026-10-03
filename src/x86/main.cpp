#include <iostream>
#include <string.h>
#include <stdio.h>

#include "custom_cloudTencent.hpp"
#include "http_check_ip_wan.h"

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

    // 本地 IP 检查
    check_demo_http_check();

    // 腾讯云接口
    cloudTencent_dnspod dnspod;

    dnspod.dnspod_domain_list();


}
