#ifndef _HTTP_CHECK_DOMAIN_API_H
#define _HTTP_CHECK_DOMAIN_API_H

#include <iostream>
#include <string>
#include "http_check_ip_wan.h"

using namespace std;

extern void api_connect_server(Ipaddress& ip);

extern void api_DNS_IPv4(string str_domain, string ip_v4);

extern void api_DNS_IPv6(string str_domain, string ip_v4);

#endif
