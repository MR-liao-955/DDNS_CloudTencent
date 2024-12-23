# 添加源文件
file(GLOB SOURCES "cJSON.c")

# 生成库文件
add_library(utiles ${SOURCES})
