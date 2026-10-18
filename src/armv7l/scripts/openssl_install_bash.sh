mkdir install_dir

 ./Configure linux-arm_32 --prefix=/home/workspace/DDNS_CloudTencent/src/armv7l/lib_src/openssl-3.4.0/install_dir

./Configure linux-armv4 \
    --cross-compile-prefix=/home/workspace/DDNS_CloudTencent/src/armv7l/tools/toolchains/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf- \
    --prefix=/home/workspace/DDNS_CloudTencent/src/armv7l/lib_src/openssl-3.4.0/install_dir \
    --openssldir=/home/workspace/DDNS_CloudTencent/src/armv7l/lib_src/openssl-3.4.0/ssl \
    no-shared

make

make install
