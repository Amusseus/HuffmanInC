/* struct used for reading bits from file*/
typedef struct BYTESTREAM{
    int readFile; /* file to read */
    unsigned char *bytes; /* holds the read bytes */
    int size; /* size of bytes */
    int count; /* number of bytes read by read() */
    int next_bit; /* next bit to read */
    int num_byte; /* number of bytes already read */
}byteStream;

/* functions for byteStream */

/* creates bystream for inFile */
byteStream *createStream(int inFile);

/* resets the bytestream so it can read again */
void resetStream(byteStream *streamer);

/* checks if the next bit is 0 or 1 */
char checkNextBit(byteStream *streamer);

/* reads from inFile */
void readStream(byteStream *streamer, int size);