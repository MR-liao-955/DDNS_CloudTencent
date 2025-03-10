### 腾讯云更新DDNS

> 概览

 腾讯云服务器要过期了，而且硬盘 80G 不够用, 带宽8M 虽然挺多,但是也就博客能用,考虑使用家用宽带的 IPv6.

 该项目不仅局限于x86, 亦适用于 Arm系列的 Linux, 但本人精力和能力有限, 仅仅提供适用我自己的架构的软件.

这部分并非完全是

> 准备工作


VMware + ubuntu1804.6 + vscode ssh
```bash
软件部分: 版本 (当前版本依赖可能存在问题)
openssl  openssl-3.4.0
curl	curl-7.58.0

证据如下：(openssl 只支持到 1.1.1)
root@dearl_ubuntu1804:/home/workspace/DDNS_CloudTencent/src/x86/lib_src/curl-7.58.0/install_dir/bin# ls
curl  curl-config
root@dearl_ubuntu1804:/home/workspace/DDNS_CloudTencent/src/x86/lib_src/curl-7.58.0/install_dir/bin# ./curl --version
curl 7.58.0 (x86-unknown-none) libcurl/7.58.0 OpenSSL/1.1.1
Release-Date: 2018-01-24
Protocols: dict file ftp ftps gopher http https imap imaps pop3 pop3s rtsp smb smbs smtp smtps telnet tftp
Features: AsynchDNS IPv6 Largefile NTLM NTLM_WB SSL TLS-SRP UnixSockets HTTPS-proxy

```


> 展望

这个 DDNS 定时软件是给我的 pve(proxmox) 使用,
同时也是给香蕉派、泰山派使用。
pve 搭建 jellyfin 影音服务器用。
香蕉派搭建 wordpress 博客 && nextcloud 网盘
再买一个电视机盒子搞个音乐服务器




#### 一、x86 架构

##### 1. 编译环境搭建

- 安装 gcc  [参考博客](https://blog.csdn.net/qq_39366020/article/details/106434094)





- vscode 远程 ssh



##### 2. makefile 编写

- Makefile 编写步骤


##### 3. SDK 文档

curl 移植 https://cloud.tencent.com/developer/article/1932820
./configure --prefix=$PWD/install_dir --host=x86 CC=gcc CXX=g++

##### 4. 报错



###### 4.1 缺少 curl 库

- 编译现象，虽然

  ```bash
  root@dearl_ubuntu1804:/home/workspace/DDNS_CloudTencent/src/x86# make
  --------- Building the final binary ---------
  main.c:11:10: fatal error: curl/curl.h: No such file or directory
   #include "curl/curl.h"
            ^~~~~~~~~~~~~
  compilation terminated.
  Makefile:48: recipe for target 'bin/ddns_cloudTencent' failed
  make: *** [bin/ddns_cloudTencent] Error 1
  root@dearl_ubuntu1804:/home/workspace/DDNS_CloudTencent/src/x86#
  ```

- 解决办法 [参考地址](https://blog.icrystal.top/archives/10.html)

  1. 由于当前系统安装了 curl , 但是我并没有找到 curl/curl.h 的头文件。因此考虑重新编译一下 curl 的链接库，由于X86 不在乎这点内存和硬盘空间，因此编译成 静态链接库 libcurl

  - 安装 openssl 库

    ```bash
    ./Configure linux-x86_64 --prefix=/home/workspace/DDNS_CloudTencent/src/x86/lib_src/openssl-3.4.0/install_dir
    doc/openssl-1.1.1
    ./Configure linux-x86_64 --prefix=/home/workspace/DDNS_CloudTencent/src/x86/lib_src/openssl-1.1.1/install_dir
    ```

  -








#### 二、arm 架构, 这里用的 arm64(泰山派) 和arm32 (电视机盒子+香蕉派 armv7l),

##### 1. 交叉编译环境搭建


##### 2. 移植 curl 库, 移植 openssh, 移植 openssl, 移植 curl, 移植ssh(如需要)
移植openssl 编译报错



#### 三、程序部分

##### 1. linux 获取本地 IP 地址
- 通过 getifaddrs 函数来获取 IPv4/6 地址 [参考地址方法5](https://blog.csdn.net/zhongmushu/article/details/89944990)
```cpp
    #include <stdio.h>
    #include <sys/types.h>
    #include <ifaddrs.h>
    #include <netinet/in.h>
    #include <string.h>
    #include <arpa/inet.h>


    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct); // 获取 IPv6地 址


````



