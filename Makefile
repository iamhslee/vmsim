# 2024-1 Operating Systems (ITP30002) - HW #4
# File  : Makefile
# Author: Hyunseo Lee (22100600) <hslee@handong.ac.kr>

# Makefile
CC = gcc
CFLAGS = -Wall -g
TARGET = vmsim
SRCS = vmsim_main.c vmsim_op.c
OBJS = $(SRCS:.c=.o)

# make  
all: $(TARGET)

# compile executable file 
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# compile object files 
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# make clean
clean:
	rm -f $(OBJS) $(TARGET)

# make rebuild
rebuild: clean all