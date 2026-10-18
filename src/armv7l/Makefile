# 编译器路径
CC := /home/workspace/DDNS_CloudTencent/src/armv7l/tools/toolchains/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++
# 编译参数
CFLAGS := -Os -g0 -DNDEBUG -Wall -lpthread

# 原始代码路径
MAIN_DIR = ./../x86
DEVELOP_DIR = $(MAIN_DIR)/develop
CURRENT_DIR = ./

OBJ_DIR = $(CURRENT_DIR)/obj

TARGET_BIN_DIR = bin

BIN_NAME = ddns_cloudTencent

# 用户定义的头文件路径
INC_PREFIX = -I
INC_USR = $(MAIN_DIR)/inc
INC_UTIL = $(MAIN_DIR)/util 							\
			$(MAIN_DIR)/develop/cloudTencent			\
			$(MAIN_DIR)/develop/cloudAlibaba 			\
			$(MAIN_DIR)/develop/rsa_configFile			\
			$(MAIN_DIR)/develop

# 外部库  openssl-1.1.1
INC_EXTERN = $(MAIN_DIR)/lib_src/curl-7.58.0/install_dir/include \
			$(MAIN_DIR)/lib_src/openssl-3.4.0/install_dir/include

# 头文件路径
ALL_INC = $(addprefix $(INC_PREFIX), $(INC_USR) $(INC_UTIL)  $(INC_EXTERN))

# 源文件列表
SRC_FILES =	$(DEVELOP_DIR)/check_ip_wan.cpp \
			$(DEVELOP_DIR)/rsa_configFile/rsa_decode.cpp \
			$(DEVELOP_DIR)/cloudTencent/custom_cloudTencent.cpp \
			$(DEVELOP_DIR)/cloudTencent/custom_cloudTencent_response_decode.cpp \
			$(DEVELOP_DIR)/cloudAlibaba/custom_cloudAlibaba.cpp \
			$(DEVELOP_DIR)/cloudAlibaba/custom_cloudAlibaba_json_decode.cpp \
			$(MAIN_DIR)/util/tretap_CJSON.c \
			$(MAIN_DIR)/util/tretap_base_64.c \
			$(MAIN_DIR)/util/curl_request_api.cpp \
			$(MAIN_DIR)/main.cpp

# 静态链接库
# LIBDIR = /usr/local/lib  # 静态库的路径
LIBDIR1 = $(CURRENT_DIR)/lib_src/openssl-3.4.0/install_dir/lib  # 静态库的路径
LIBDIR2 = $(CURRENT_DIR)/lib_src/curl-7.58.0/install_dir/lib  # 静态库的路径
# LIBS =  -lcurl  -lssl  -lcrypto -ldl -lpthread  # 静态库的名称（libmystatic.a）
LIBS = -Wl,-Bstatic \
       -lcurl -lssl -lcrypto \
       -Wl,-Bdynamic \
       -ldl -lpthread

# 目标文件列表
OBJ_FILES = $(SRC_FILES:$(SRC_FILES)=$(OBJ_DIR)/%.o)

# 可执行文件
TARGET = $(TARGET_BIN_DIR)/$(BIN_NAME)

# 目标文件生成规则
$(OBJ_DIR)/%.o: $(SRC_FILES)
	@echo "------------- Compiling $< --------------"
	$(CC) $(CFLAGS)  $(ALL_INC) -o $@ -c $<

# 生成最终可执行文件
$(TARGET): $(OBJ_FILES)
	@echo "--------- Building the final binary ---------"
	@$(CC) $(CFLAGS) $(ALL_INC)  -L$(LIBDIR1) -L$(LIBDIR2) $^ $(LIBS)  -o $@

# 创建 src 子目录
prep:
	@if test ! -d $(OBJ_DIR); then mkdir $(OBJ_DIR); fi
	@if test ! -d $(TARGET_BIN_DIR); then mkdir $(TARGET_BIN_DIR); fi

# 清理目标和中间文件
clean:
	@rm -rf $(OBJ_DIR) $(TARGET_BIN_DIR)/$(BIN_NAME)

# 默认目标
all: prep $(TARGET)
