### 腾讯云更新DDNS

> 概览

 腾讯云服务器要过期了，而且硬盘 80G 不够用, 带宽8M 虽然挺多,但是也就博客能用,考虑使用家用宽带的 IPv6.

 该项目不仅局限于x86, 亦适用于 Arm系列的 Linux, 但本人精力和能力有限, 仅仅提供适用我自己的架构的软件.

这部分并非完全是

> 准备工作


VMware + ubuntu1804.6 + vscode ssh
```bash
软件部分: 版本
openssl
curl


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


#### 二、arm 架构, 这里用的 arm64(泰山派) 和arm32 (电视机盒子+香蕉派 armv7l),

##### 1. 交叉编译环境搭建


##### 2. 移植 curl 库, 移植 openssh, 移植 openssl, 移植 curl, 移植ssh(如需要)
移植openssl 编译报错

