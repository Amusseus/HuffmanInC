#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "streamReader.h"
#define SIZE 4096

/* create bytestream */
byteStream *createStream(int inFile){
    byteStream *tempStream = (byteStream *)malloc(sizeof(byteStream));
    tempStream->readFile = inFile;
    tempStream->size = SIZE;
    tempStream->bytes = (unsigned char *)malloc(SIZE * sizeof(unsigned char));
    /* reads from inFile */
    tempStream->count = read(inFile,tempStream->bytes,SIZE);
    tempStream->next_bit = 7; /* read from lsb */
    tempStream->num_byte = 0; 
    return tempStream;
}

/* reset bytestream so it can read again */
void resetStream(byteStream *streamer){
    streamer->next_bit = 7;
    streamer->num_byte = 0;
}


char checkNextBit(byteStream *streamer){
    char optRet; /* the bit to be returned */
    /* refrence to stramer->bytes */
    unsigned char *byteREf = streamer->bytes;
    /* used & operation to check if bit is 0 or 1 */
    optRet = byteREf[streamer->num_byte] & (1UL << streamer->next_bit);
    streamer->next_bit --;

    /* if next a whole byte has been read, then increment to next byte */
    if(streamer->next_bit == -1 ){
        streamer->next_bit = 7;
        streamer->num_byte++;
    }

    /* if the whole buffer has been read, then reset and read again */
    if(streamer->num_byte == streamer->size){
        resetStream(streamer);
        readStream(streamer, SIZE);
    }

    /* return the bit 0 or 1 */
    return optRet;
}

/* read from inFile */
void readStream(byteStream *streamer, int size){
    read(streamer->readFile,streamer->bytes,size);
}