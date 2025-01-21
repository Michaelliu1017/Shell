
# Makefile 10/5/24
CC = gcc

# 编译指令
CFLAGS = -Wall -g

#可执行文件名
TARGET = shell

# source文件
SRC = main.c helper.c function.c  

#生成exec
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# 清理
clean:
	rm -f $(TARGET)
