/* struct used for writing to file */
typedef struct BYTESTREAM{
    int printFile; /* file to print to */
    unsigned char *bytes; /* holds the bytes to write */
    int size; /* size of bytes */
    int next_bit; /* next bit to write to */
    int num_byte; /* number of bytes completed */
}byteStream;

/* functions for byteStream */

/* creates byteStream for outFile */
byteStream *createStream(int outFile);

/* reset bytestream so it can write again */
void resetStream(byteStream *streamer);

/* set the next bit to be written */
void setNextBit(byteStream *streamer, char opt);

/* write the stream to outFile */
void writeStream(byteStream *streamer, int size);