#include <iostream>
#include <string.h>
#include <stdio.h>

#include "custom_cloudTencent.hpp"
#include "http_check_ip_wan.h"



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
    check_demo_http_check();

    cloudTencent_dnspod dnspod;

    dnspod.domain_id = DOMAIN_ID;
    // dnspod.login_token = LOGIN_TOKEN;
    // dnspod.record_id = ;
    dnspod.record_id = RECORD_ID;

    dnspod.get_record_info();


    // custom_cloudTencent_init();
}
