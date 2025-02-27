# 编译器路径
CC := /usr/bin/gcc
# 编译参数
CFLAGS := -Os -g0 -DNDEBUG -Wall -lpthread

# 原始代码路径
MAIN_DIR = .
SRC_DIR = $(MAIN_DIR)/src

OBJ_DIR = $(MAIN_DIR)/obj

TARGET_BIN_DIR = bin

BIN_NAME = ddns_cloudTencent

# 用户定义的头文件路径
INC_PREFIX = -I
INC_USR = $(MAIN_DIR)/inc
INC_UTIL = $(MAIN_DIR)/util

# 头文件路径
ALL_INC = $(addprefix $(INC_PREFIX), $(INC_USR) $(INC_UTIL))

# 源文件列表
SRC_FILES = $(SRC_DIR)/https_request.c \
			$(SRC_DIR)/upstream_encode.c \
			$(MAIN_DIR)/util/test.c \
			$(MAIN_DIR)/main.c

# 目标文件列表
OBJ_FILES = $(SRC_FILES:$(SRC_FILES)=$(OBJ_DIR)/%.o)
# OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# 可执行文件
TARGET = $(TARGET_BIN_DIR)/$(BIN_NAME)

# 目标文件生成规则
$(OBJ_DIR)/%.o: $(SRC_FILES)
	@echo "------------- Compiling $< --------------"
	$(CC) $(CFLAGS)  $(ALL_INC) -o $@ -c $<

# $(OBJ_DIR)/%.o : $(MAIN_DIR)/%.c
# 	@echo "------------- 3 Compiling $< --------------"
# 	$(CC) $(CFLAGS)  $(ALL_INC) -o $@ -c $<

# $(OBJ_DIR)/%.o : $(MAIN_DIR)/src/%.c
# 	@echo "------------- 2 Compiling $< --------------"
# 	$(CC) $(CFLAGS)  $(ALL_INC) -o $@ -c $<

# $(OBJ_DIR)/%.o: $(MAIN_DIR)/util/%.c
# 	@echo "------------- 1 Compiling $< --------------"
# 	$(CC) $(CFLAGS)  $(ALL_INC) -o $@ -c $<

# 生成最终可执行文件
$(TARGET): $(OBJ_FILES)
	@echo "--------- Building the final binary ---------"
	@$(CC) $(CFLAGS) $(ALL_INC) -o $@ $^


# $(OBJ_DIR)/%.o:$(MAIN_DIR)/%.c
# 	@echo "------------- $(MAIN_DIR) generate --------------"
# 	$(CC) --sysroot=$(SYSROOT) $(CFLAGS)  $(ALL_INC) -o $@ -c $<

# $(OBJ_DIR)/%.o:$(MAIN_DIR)/src/%.c
# 	@echo "------------- $(SRC_DIR) generate --------------"
# 	$(CC) --sysroot=$(SYSROOT) $(CFLAGS) $(ALL_INC) -o $@ -c $<


# 创建目标目录
# prep:
# 	@if test ! -d $(TARGET_BIN_DIR); then mkdir $(TARGET_BIN_DIR); fi
# 	@if test ! -d $(OBJ_DIR); then mkdir $(OBJ_DIR); fi

# 创建 src 子目录
prep:
	@if test ! -d $(OBJ_DIR); then mkdir $(OBJ_DIR); fi
	@if test ! -d $(TARGET_BIN_DIR); then mkdir $(TARGET_BIN_DIR); fi
# @if test ! -d $(OBJ_DIR)/src; then mkdir $(OBJ_DIR)/src; fi
# @if test ! -d $(OBJ_DIR)/util; then mkdir $(OBJ_DIR)/util; fi



# 清理目标和中间文件
clean:
	@rm -rf $(OBJ_DIR) $(TARGET_BIN_DIR)

# 默认目标
all: prep $(TARGET)
