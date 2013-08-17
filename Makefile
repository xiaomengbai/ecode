SOURCE=main.c index.c rabin.c meta.c ecode.c matrix_code.c galois.c col.c common.c sha1.c md5.c 
TARGET=main


default:
	gcc ${SOURCE} -o ${TARGET} -lm -lpthread -O3 #-g -O3

clean:
	rm *~ ${TARGET}
