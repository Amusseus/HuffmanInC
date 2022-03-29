#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "streamWriter.h"
#define SIZE 4096

/* functions for byteStream */

/* create a byteStream for a file */
byteStream *createStream(int outFile){
    byteStream *tempStream = (byteStream *)malloc(sizeof(byteStream));
    tempStream->printFile = outFile;
    tempStream->bytes = (unsigned char *)calloc(SIZE,sizeof(unsigned char));
    tempStream->size = SIZE;
    /* starts writting the bits from lsb */
    tempStream->next_bit = 7;
    tempStream->num_byte = 0;
    return tempStream;
}

/* reset the byteStream */
void resetStream(byteStream *streamer){
    streamer->next_bit = 7;
    streamer->num_byte = 0;
    /* reset bytes to have all zero */
    free(streamer->bytes);
    streamer->bytes = (unsigned char *)calloc(SIZE,sizeof(unsigned char));
}

/* set the next bit in the byteStream, given that its not full */
void setNextBit(byteStream *streamer, char opt){
    /* refrence to streamer->bytes */
    unsigned char *byteRef = streamer->bytes;
    /* clear the bit */
    if(opt == '0'){
        byteRef[streamer->num_byte] &= ~(1UL << streamer->next_bit);
    }
    /* set the bit */
    else if( opt == '1'){
        byteRef[streamer->num_byte] |= (1UL << streamer->next_bit);
    }
    streamer->next_bit--;

    /* if a full byte has been "written", then increment to next byte */
    if(streamer->next_bit == -1 ){
        streamer->next_bit = 7;
        streamer->num_byte++;
    }
    /* if streamer->num_byte fills up, write and then reset */
    if(streamer->num_byte == streamer->size){
        writeStream(streamer,streamer->size);
        resetStream(streamer);
    }
}

void writeStream(byteStream *streamer, int size){
    /* write to outFile */
    write(streamer->printFile,streamer->bytes,size);
}