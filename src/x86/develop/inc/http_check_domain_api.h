#ifndef _HTTP_CHECK_DOMAIN_API_H
#define _HTTP_CHECK_DOMAIN_API_H

#include <iostream>
#include <string>

using namespace std;

void api_connect_server();

void api_DNS_IPv4(string str_domain, string ip_v4);

void api_DNS_IPv6(string str_domain, string ip_v4);

#endif
