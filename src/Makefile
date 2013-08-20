.PHONY : main

CC = gcc
INC_DIR = ../inc
CFLAGS = -O3 -I$(INC_DIR) #-Wall 
LIBS = -lpthread -lm

OBJS = main.o col.o common.o ecode.o galois.o index.o matrix_code.o md5.o meta.o rabin.o

DIFFOBJ = diff.o

TARGET = prog

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

clean:
	rm *.o $(TARGET) 