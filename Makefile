SHELL = /bin/sh
CC = gcc
CFLAGS = -Wall -ansi -pedantic
LD = gcc
LDFLAGS =
ENCODE = hencode
DECODE = hdecode
NENCODEDIR = ~pn-cs357/demos/hencode
NENCODETEST = ./nicoEncodeTest
SPECASE = 05_binary

all: $(ENCODE) $(DECODE)


$(ENCODE): $(ENCODE).o utility.o streamWriter.o
	$(LD) $(LDFLAGS) -g -o $(ENCODE) $(ENCODE).o utility.o streamWriter.o

$(DECODE): $(DECODE).o utility.o streamReader.o
	$(LD) $(LDFLAGS) -g -o $(DECODE) $(DECODE).o utility.o streamReader.o

$(ENCODE).o: $(ENCODE).c
	$(CC) $(CFLAGS) -g -c -o $(ENCODE).o $(ENCODE).c

$(DECODE).o: $(DECODE).c
	$(CC) $(CFLAGS) -g -c -o $(DECODE).o $(DECODE).c

utility.o: utility.c
	$(CC) $(CFLAGS) -g -c -o utility.o utility.c

streamWriter.o: streamWriter.c
	$(CC) $(CFLAGS) -g -c -o streamWriter.o streamWriter.c

streamReader.o: streamReader.c
	$(CC) $(CCFLAGS) -g -c -o streamReader.o streamReader.c

runEncode: $(ENCODE)
	@./$(ENCODE) $(NENCODETEST)/$(SPECASE) > myTest.out
	@xxd myTest.out

testEncode: $(ENCODE)
	@./$(ENCODE) $(NENCODETEST)/$(SPECASE) > myTest.out
	@$(NENCODEDIR) $(NENCODETEST)/$(SPECASE) > nicoTest.out
	diff nicoTest.out myTest.out

testEncodeAll:
	@./tryAsgn3

cleanTest:
	rm *.out

clean:
	rm *.o


