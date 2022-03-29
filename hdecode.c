#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "utility.h"
#include "streamReader.h"
#define OPTION 256
#define SIZE 4096

/* functions for use only in hencode */

/*takes input from command line */
void takeInput(int argc, char *argv[], int *inFile, int *outFile);
/* reads header from file, and returns a huffman tree */
int readHeader(int inFile, int *uniqC, node **tree);
/* decodes the file and writes to the outfile */
void writeBody(int inFile, int outFile, int uniqC, node *tree, 
    unsigned int numRead);

/* Usage: hencode [ (infile | -) [outFile]] */

int main(int argc, char *argv[]){
    int inFile; /* file to read */
    int outFile; /* file to write */
    int numUniq; /* uniq num of chars present */
    unsigned int charToRead; /* total number of chars to read */
    node *tree; /* huffman tree to read from */

    /* gets input from command line */
    takeInput(argc,argv,&inFile,&outFile);
    /* generates the huffman tree */
    charToRead = readHeader(inFile, &numUniq, &tree);
    /* decode the file */
    writeBody(inFile,outFile,numUniq,tree,charToRead);

    return 0;
}

void takeInput(int argc, char *argv[], int *inFile, int *outFile){
    /* opening file to read */
    if(argc > 1 ){
        /* if - argument, then take stdin */
        if(strcmp(argv[1],"-") == 0){
            *inFile = STDIN_FILENO;
        }
        /* if file cannot be opened */
        else if((*inFile = open(argv[1],O_RDONLY)) == -1){
            perror("nonexistant: No such file or directory");
            exit(1);
        }
    }
    /* no Input given, print Usage error */
    else{
        perror("usage hdecode [ (infile | -) [outFile]]");
        exit(1);
    }

    /* opening file to write*/
    if(argc > 2){
        /* if optional file given */
        *outFile = open(argv[2],O_CREAT | O_WRONLY, S_IRWXU);
    }
    /* if file not present then print to stdout */
    else{
        *outFile = STDOUT_FILENO;
    }

}

/* read inFile and generate huffman tree */
int readHeader(int inFile, int *uniqC, node **tree){
    int i; /* used to loop */
    unsigned char c; /* used to read one byte */
    unsigned int integer; /* used to read 4 bytes */
    int numChar; /* num of uniq characters */
    int totalCharRead; /* total num of chars to read in whole file */

    /* data structures used to generate trees */
    counter *counterList;
    node *linkedList;
    
    /* getting numChars from first byte */
    if(read(inFile,&c,sizeof(unsigned char)) == 0){
        /* return 0 total char to read, and set tree to NULL */
        *tree = NULL;
        return 0;
    }
    numChar = c + 1;
    *uniqC = numChar;

    /* create a counter for each char, input it into list */
    i = 0;
    totalCharRead = 0;
    counterList = (counter *)malloc(sizeof(counter) * numChar);
    /* traversing the header */
    while(i < numChar){
        read(inFile,&c,sizeof(unsigned char)); /* read the char */
        read(inFile,&integer,sizeof(int)); /* read the char's frequency */
        totalCharRead += htonl(integer); /* increment total num to read */
        /* add to list */
        counterList[i] = *(createCounterSize(c,htonl(integer)));
        i++;
    }

    /* sort the list of counters */
    qsort(counterList,numChar,sizeof(counter),compareCounter);
    /* generate linked list of counters */
    linkedList = generateLinked(counterList,numChar);
    /* create huffman tree */
    *tree = generateTree(&linkedList,numChar);

    return totalCharRead;
}

/* use huffman tree to decode the file */
void writeBody(int inFile, int outFile, int uniqC, node *tree,
    unsigned int numRead){
    /* used to write to outFile */
    int writeIndex;
    unsigned char *writer;
    /* used to read infile */
    unsigned int numReader;
    byteStream *reader = createStream(inFile);
    /* used to traverse huffman tree */
    node *current;

    /* while reader has read atleast 1 character */
    current = tree;
    numReader = numRead;

    /* if only one character exists */
    if(reader->count == 0 && numRead != 0){
        int i = 0;
        /* create buffer of size numRead */
        writer = (unsigned char*)malloc(sizeof(unsigned char) * numRead);
        /* change every byte in buffer to char for printing */
        while(i < numRead){
            writer[i] = tree->car.c ;
            i++;
        }
        /* write the buffer to outFile */
        write(outFile, writer, numRead *sizeof(unsigned char));
    }

    /* set up writing buffer */
    writeIndex = 0;
    writer = (unsigned char*)malloc(sizeof(unsigned char) * SIZE);
    /* while reader has read atleast one char */
    while(reader->count > 0){
        /* if infile isn't empty */
        if(numReader != 0){
            char opt; /* the bit, 0 or 1 */
            /* check next bit */
            opt = checkNextBit(reader);
            /* based on the bit, get the corresponding node */
            current = getNextNode(current,opt);
            /* if leaf node is reached */
            if(nodeIsLeaf(current)){
                /* add to write buffer */
                writer[writeIndex] = current->car.c;
                writeIndex++;
                /* if write buffer fills up then write to outFile */
                if(writeIndex == SIZE){
                    write(outFile, writer, SIZE * sizeof(char));
                    writeIndex = 0;
                }
                /* reset current to top of tree */
                current = tree;
                /* decrement characters left to be read */
                numReader--;
            }
        }
        /* break loop and leave if infile is empty */
        else{
            break;
        }
    }
    /* if write buffer contains bytes to write */
    if(writeIndex > 0){
        /* writes the num of bytes left in buffer to outFile */
        write(outFile, writer, writeIndex * sizeof(char));
    }
}