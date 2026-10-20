cd /home/workspace/DDNS_CloudTencent/src/armv7l/lib_src/curl-7.58.0

mkdir install_dir

# 交叉编译工具链前缀：替换成实际存在的路径
export CROSS_COMPILE=/home/workspace/DDNS_CloudTencent/src/armv7l/tools/toolchains/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-

export CC="${CROSS_COMPILE}gcc"
export CXX="${CROSS_COMPILE}g++"
export AR="${CROSS_COMPILE}ar"
export RANLIB="${CROSS_COMPILE}ranlib"
export STRIP="${CROSS_COMPILE}strip"

# ARM 版依赖库目录
export OPENSSL_DIR=/home/workspace/DDNS_CloudTencent/src/armv7l/lib_src/openssl-3.4.0/install_dir
export CURL_DIR=/home/workspace/DDNS_CloudTencent/src/armv7l/lib_src/curl-7.58.0/install_dir


./configure \
		--build=x86_64-pc-linux-gnu \
		--host=arm-linux-gnueabihf \
		--prefix="$CURL_DIR" \
		--with-ssl="$OPENSSL_DIR" \
		--with-ca-bundle=/etc/ssl/certs/ca-certificates.crt \
    	--with-ca-path=/etc/ssl/certs \
		--enable-shared \
		--enable-static \
		--disable-ldap \
		--disable-ldaps \
		--without-zlib \
		CC="$CC" \
		AR="$AR" \
		RANLIB="$RANLIB" \
    	LIBS="-ldl -lpthread"

make

make install
