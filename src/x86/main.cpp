#include <iostream>
#include <string.h>
#include <stdio.h>

#include "custom_cloudTencent.h"
#include "http_check_ip_wan.h"

int main() {
    std::cout << "hello world ! ~~~" << std::endl;
    check_demo_http_check();

    custom_cloudTencent_init();
}
