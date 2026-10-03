#include <stdio.h>
#include <string.h>
#include <string>

#include "custom_cloudTencent.hpp"
#pragma once
using namespace std;

#define _RETURN_SUCC 0
#define _RETURN_FAIL -1

bool tencentcloud_decode_domain_list(string src, cloudTencent_dnspod *dnspod);

bool tencentcloud_decode_record_list(string src, cloudTencent_dnspod *dnspod);

bool tencentcloud_decode_record_list_filter(string src, cloudTencent_dnspod *dnspod);
