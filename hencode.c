#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "utility.h"
#include "streamWriter.h"
#define OPTION 256 /* num of chars */
#define SIZE 4096  /* size of reader */

/* functions for use only in hencode */

/* takes input from command line */
void takeInput(int argc, char *argv[], int *inFile, int *outFile);

/* generates a list of huffmanCodes for each avalaiable char */
int generateHCFromFile(int readfile, counter **list, char **listCodes);

/* write the header for huffFile */
void writeHeader(int outFile, int uniqChar, counter **list);

/*write the body for the  huffFile */
void writeBody(int outFile, int inFile, char **huffmanCodes);

/* Usage: hencode infile [outfile]
* optional outfile, if not present then print to stdout */

/* program takes argument from command line (up to 2)*/
int main(int argc, char *argv[]){
    int inFile; /* file to read */
    int outFile; /* file to write */
    int numChars; /* num of uniq characters */
    
    /* important lists needed for Encoding */
    counter *charList[OPTION] ={NULL}; /* all avaliable chars */
    char *huffmanCodes[OPTION] = {NULL}; /* huffcodes for chars */

    /* gets input from command line */
    takeInput(argc,argv,&inFile,&outFile);
    /* generates Huffman Codes for the inFile */
    numChars = generateHCFromFile(inFile, charList, huffmanCodes);
    /* resets the position of the file to start */
    close(inFile);
    inFile = open(argv[1],O_RDONLY);
    /* if file isn't empty then encode */
    if(numChars > 0){
        /* Encode the Header and Body */
        writeHeader(outFile, numChars,charList);
        writeBody(outFile,inFile,huffmanCodes);
    }

    /* end program */
    return 0;
}

void takeInput(int argc, char *argv[], int *inFile, int *outFile){
    /* opening file to read */
    if(argc > 1){
        *inFile = open(argv[1], O_RDONLY);
        /* File does not exist error */
        if(*inFile == -1){
            perror("nonexistant: No such file or directory");
            exit(1);
        }
    }
    /* No files given results in Usage error */
    else{
        perror("usage hencode infile [ outfile ]");
        exit(1);
    }

    /* opening file to write*/
    if(argc > 2){
        /* if file doesn't exist, then create to write */
        *outFile = open(argv[2], O_CREAT | O_WRONLY, S_IRWXU);
    }
    /* if no outfile given, then print to stdout */
    else{
        *outFile = STDOUT_FILENO;
    }

}

int generateHCFromFile(int readfile, counter **list, char **listCodes){
    /* ints for looping */
    int i;
    int listIndex;
    /* lists and data structues need to generate codes */
    counter *hashMap[OPTION] = {NULL}; /* holds all possible chars */
    counter *counterList; /* holds existing counters */
    node *linkedList;
    node *tree;
    char huffString[30]; /* helps create huff codes */
    int hashSize; /* num of uniq chars, will be returned */
    /* vars used to read file */
    unsigned char c;
    unsigned char *charStream;
    int count; 

    /* Read characters from File */
    i = 0;
    hashSize = 0;
    charStream = (unsigned char *)malloc(sizeof(unsigned char) * SIZE);
    /* while open can read one or more characters */
    while( (count = read(readfile,charStream, SIZE)) > 0 ){
        /* read char from the stream*/
        while(i < count){
            /* get individual char */
            c = charStream[i];
            if(c >= 0){
                /* add counter if doesn't increase, increase size */
                if((hashMap[c]) == NULL){
                    hashMap[c] = createCounter(c);
                    list[c] = createCounter(c);
                    hashSize++;
                }
                /* increase frequency if counter exists */
                else{
                    freqUp(hashMap[c]);
                    freqUp(list[c]);
                }
            }
            i++;
        }
        /* reset stream once its done being read*/
        i = 0;
    }

    /* if the file isn't empty */
    if( hashSize > 0){

        /* creates a list of only counters, no NULLS */
        i = 0;
        listIndex = 0;
        /* allocs enough space for list size hashSize*/
        counterList = (counter *)malloc(hashSize * sizeof(counter));
        while(i < OPTION){
            /* if counter exists then add it to list */
            if(hashMap[i] != NULL){
                counterList[listIndex] = *hashMap[i];
                listIndex++;
            }
            i++;
        }

        /* sorting the list of counters in ascending order */
        qsort(counterList,hashSize,sizeof(counter),compareCounter);
        /* converting the list of Counters into linked list */
        linkedList = generateLinked(counterList,hashSize);
        /* generates huffman tree from linked list */
        tree = generateTree(&linkedList,hashSize);
        /* generates huffman codes from huffman tree */
        getAllHuffCodes(tree,listCodes,huffString, 0);
    }
    return hashSize;
}

void writeHeader(int outFile, int uniqChar, counter **list){
    /* format of header goes as follows 
    * 1. BYTE (n-1) num of uniq characters in file 
    * 2. BYTE for character it self
    * 3. UNSIGNED INT(4 BYTE) for its frequency
    * 4. repeat 2 & 3 for all characters in file */
    int i = 0;
    unsigned char numChar = uniqChar -1;
    write(outFile,&numChar,sizeof(char));
    while(i < OPTION){
        if(list[i] != NULL){
            char Charz = list[i]->c;
            /* converting from host to network byte order */
            unsigned int freq = htonl(list[i]->count);
            /* write char and its frequency */
            write(outFile,&Charz, sizeof(char));
            write(outFile,&freq,sizeof(unsigned int));
        }
        i++;
    }

}

void writeBody(int outFile, int inFile, char **huffmanCodes){
    /* int used for looping */
    int i;
    int j;
    /* used for reading file */
    unsigned char c;
    unsigned char *charStream;
    int count;
    /* used for writing file */
    int endWriter;
    byteStream *writer;

    /* Reading from inFile */
    i = 0;
    charStream = (unsigned char *)malloc(sizeof(unsigned char) * SIZE);
    writer = createStream(outFile);
    /* while read() can read atleast one character */
    while( (count = read(inFile,charStream, SIZE)) > 0 ){
        /* read character from stream */
        while(i < count){
            c = charStream[i];
            if(c >= 0){
                /* get huffman Code for char */
                char *charCode = huffmanCodes[c];
                j = 0;
                /* traverse huffman Code */
                while(j < strlen(charCode)){
                    if(charCode[j] == '0'){
                        setNextBit(writer, '0');
                    }
                    else if(charCode[j] == '1'){
                        setNextBit(writer, '1');
                    }
                    j++;
                }
            }
            i++;
        }
        /* reset stream */
        i = 0;
    }

    /* print remaining bits if neccessary */
    endWriter = writer->num_byte;
    if(writer->next_bit < 7){
        endWriter++;
    }
    writeStream(writer,endWriter);
}