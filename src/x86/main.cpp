#include <iostream>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "custom_cloudTencent.hpp"
#include "check_ip_wan.hpp"

// 用于 json 解析库使用。
extern void *ddns_mem_alloc(int size);
extern void ddns_mem_free(void **ptr);

config_t g_file_config;

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


static void file_config_read(void){

    /* 读取文件中的 keyid 和 key */
    std::stringstream file_read;
    std::ifstream file(CONFIG_PATH);
    file_read << file.rdbuf();
    std::string config = file_read.str();
    std::stringstream ss(config);
    std::string line;
    while (std::getline(ss, line)) {
        // Windows 文本文件可能存在 \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        // ';' 是配置项结束符
        size_t pos = line.find(';');
        if (pos != std::string::npos) {
            line = line.substr(0, pos);
        }

        // sub
        if (line.compare(0, 12, "domain_name=") == 0) {
            g_file_config.domain_name = line.substr(12);
        }
        else if (line.compare(0, 15, "domain_subname=") == 0) {
            g_file_config.domain_subname = line.substr(15);
        }
        else if (line.compare(0, 18, "check_ip_url_ipv4=") == 0) {       // 监测当前 IP 是否是公网IP的网址
            g_file_config.check_ip_url_ipv4 = line.substr(18);
        }
        else if (line.compare(0, 18, "check_ip_url_ipv6=") == 0) {       // 监测当前 IP 是否是公网IP的网址
            g_file_config.check_ip_url_ipv6 = line.substr(18);
        }

    }

    if(!g_file_config.domain_subname.empty())
        g_file_config.domain_full = g_file_config.domain_subname + "." + g_file_config.domain_name;
    else
        g_file_config.domain_full = g_file_config.domain_name;

    printf("[%s] domain_name = %s\n", __func__, g_file_config.domain_name.c_str());
    printf("[%s] domain_subname = %s\n", __func__, g_file_config.domain_subname.c_str());
    printf("[%s] domain_full = %s\n", __func__, g_file_config.domain_full.c_str());
    printf("[%s] check_ip_url_ipv4 = %s\n", __func__, g_file_config.check_ip_url_ipv4.c_str());
    printf("[%s] check_ip_url_ipv6 = %s\n", __func__, g_file_config.check_ip_url_ipv6.c_str());

}

int main() {

    std::cout << "============= ddns tool [version:"<< VERSION <<"] ===========" << std::endl;

    file_config_read();

    _IPv4 &ip_v4 = _IPv4::getInstance(g_file_config.domain_full);
    _IPv6 &ip_v6 = _IPv6::getInstance(g_file_config.domain_full);

    // 本地 IP 检查
    check_demo_http_check();

    // 腾讯云接口
    cloudTencent_dnspod &dnspod = cloudTencent_dnspod::getInstance();

    dnspod.dnspod_domain_list();

    // dnspod.dnspod_record_list();         // 此函数暂时无作用, 使用 filter 代替。
    // dnspod.dnspod_record_line_list();

    dnspod.dnspod_record_list_filter();

    dnspod.record_delete_remain_one();

    // 如果是 本地IP 是公网 IP
    if(_RETURN_SUCC == ip_v4.match_wanip_with_localip()){
        dnspod.dnspod_record_modify(cloudTencent_dnspod::CLASS_IPV4, ip_v4.ip_ddns_modify);
    }

    if(_RETURN_SUCC == ip_v6.match_wanip_with_localip()){
        dnspod.dnspod_record_modify(cloudTencent_dnspod::CLASS_IPV6, ip_v6.ip_ddns_modify);
    }

}
