MAIN_DIR = .

SYSROOT := $(MAIN_DIR)/../output/d211_fountainhead_demo128/host/riscv64-linux-gnu/sysroot
CC := $(CROSS_COMPILE)gcc
CFLAGS := -Os -g0 -DNDEBUG -Wall  -lpthread
# CFLAGS := -Os -g0 -DNDEBUG -Wall  -lpthread -std=gnu99

# SRC_DIR = /root/RISC-V/linux-sdk/d211/d213_app
SRC_DIR = $(MAIN_DIR)/src

OBJ_DIR = obj

TARGET_BIN_DIR = bin

BIN_NAME = gpio_demo

#用户定义的头文件路径
INC_PREFIX = -I
INC_USR = $(MAIN_DIR)/inc
# 系统调用库头文件路径
# SYS_INC += $(MAIN_DIR)/TODO
INC_JSON = /home/json-c/install/include/json-c

# ALL_INC = $(addprefix $(INC_PREFIX), $(INC_USR))
ALL_INC = $(addprefix $(INC_PREFIX), $(INC_USR) $(INC_JSON))
# ALL_INC = $(addprefix $(INC_PREFIX), $(INC_USR), $(SYS_INC))
# ALL_INC = -I $(INC_USR)

# BIN_OBJS := test_gpio.o
BIN_OBJS = $(OBJ_DIR)/config.o \
			$(OBJ_DIR)/task_mqtt.o \
			$(OBJ_DIR)/task_uart.o \
			$(OBJ_DIR)/task_i2c.o \
			$(OBJ_DIR)/util.o \
			$(OBJ_DIR)/cmd.o \
			$(OBJ_DIR)/main.o



# $(OBJ_DIR)/util.o:$(MAIN_DIR)/src/util.c
# 	@echo "------------- $(MAIN_DIR) generate --------------"
# 	$(CC) --sysroot=$(SYSROOT) $(CFLAGS) $(ALL_INC) -o $@ -c $<

$(OBJ_DIR)/%.o:$(MAIN_DIR)/%.c
	@echo "------------- $(MAIN_DIR) generate --------------"
	$(CC) --sysroot=$(SYSROOT) $(CFLAGS)  $(ALL_INC) -o $@ -c $<

$(OBJ_DIR)/%.o:$(MAIN_DIR)/src/%.c
	@echo "------------- $(SRC_DIR) generate --------------"
	$(CC) --sysroot=$(SYSROOT) $(CFLAGS) $(ALL_INC) -o $@ -c $<

#添加了动态链接库路径，貌似没有报错了。
LIBS =  -lmosquitto
JSON_LIB = -ljson-c
#main在引入MAIN_DIR的config.h，以解决config.h报错的问题
# $(CC) --sysroot=$(SYSROOT) $(CFLAGS) -I $(MAIN_DIR) -o $@ -c $<


all:prep bin
prep:
	@if test ! -d $(TARGET_BIN_DIR); then mkdir $(TARGET_BIN_DIR); fi
	@if test ! -d $(OBJ_DIR); then mkdir $(OBJ_DIR); fi


# build:$(OBJ_DIR)/%.o


bin:$(TARGET_BIN_DIR)/$(BIN_NAME)

#添加了动态链接库路径 '-L/root/RISC-V/linux-sdk/d211/d213_app/src '
#添加静态链接库
$(TARGET_BIN_DIR)/$(BIN_NAME):$(BIN_OBJS)
	@echo "--------- bin file establish ---------"
	@$(CC)  --sysroot=$(SYSROOT) -L/root/RISC-V/linux-sdk/d211/d213_app/src -L/home/json-c/install/lib $(CFLAGS) -o $@ $^ $(LIBS) $(JSON_LIB)
# @$(CC)  --sysroot=$(SYSROOT)  $(CFLAGS) $^ -o $@

# bin:$(BIN_OBJS) # 可以正确生成但: .o文件会生成在根目录下。
# 	echo "派大星"
# 	@$(CC)  --sysroot=$(SYSROOT) $(CFLAGS) $^ -o $(TARGET_BIN_DIR)/$(BIN_NAME)

clean:
	@rm -fr $(OBJ_DIR) $(TARGET_BIN_DIR)
