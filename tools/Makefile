.PHONY : diff

CC = gcc
INC_DIR = ../inc
CFLAGS = -O3 -I$(INC_DIR) #-Wall 
LIBS = -lpthread -lm

DIFFOBJ = diff.o
DIFFTARGET = diff


diff: $(DIFFOBJ)
	$(CC) $(CFLAGS) $(DIFFOBJ) -o $(DIFFTARGET) 

clean:
	rm *.o  