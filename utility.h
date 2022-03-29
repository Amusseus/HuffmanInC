/* Includes structs and methods to implement linked list and binary trees 
*  as needed for Huffman Encoding and Decoding */

/* struct used to store a char and its frequency */
typedef struct Counter{
    unsigned int c;
    unsigned int count;
}counter;

/* struct Node used for both binary tree and linked list */
typedef struct Node{
    /* counter holds the char and count
    *  if car holds char -1, its used as placeholder */
    counter car; 

    /* used for linked list */
    struct Node *next;
    struct Node *prev;

    /* used for binary tree */
    struct Node *left;
    struct Node *right;
}node;

/* functions using node as linked list */

/* creates a linked list from array & array size 
* returns the head node of the list */
node *generateLinked(counter *countList, int listSize);

/* inserts a node into linked list in order, changes
* the pointer to head node if needed */
void insert(node **linkedlist, node *insert);

/* removes the head of linked list and returns it
* changes the head node pointer to the next node if it exists */
node *takeOut(node **linkedlist);

/* prints the linked list to stdout, takes in head node
* Used for testing purpose only */
void printLinkedList(node *head);


/* functions for the Huffman tree */

/* takes a linked list and generates a binary tree
* returns the head node of the tree */
node *generateTree(node **linkedlist, int size_linked);

/* generates huffman codes for each character in the tree
* takes the head tree node, the array of strings to hold the code, 
* a string and int to help build the code */
void getAllHuffCodes(node *tree, char **listCodes, char *huffCode, int count );

/* used to check is given node is a leaf in the tree 
* returns 1 for true, and 0 for false */
int nodeIsLeaf(node *tree);

/* gets the next node in the tree, 0 is left, 1 is right */
node *getNextNode(node *current, char opt);

/* does binary tree inorder traversal 
* used for testing purpose only */
void printInorder(node *tree);


/* functions for Counter struct */

/* takes a character and returns a counter struct */
counter *createCounter(int ch);

/* takes a char and returns a counter with count of freq */
counter *createCounterSize(int ch, int freq);

/* increases the count of a given counter */
void freqUp(counter *ref);

/* prints the counter, used for testing purpose */
void printCounter(counter *ref);

/* compare functions for counter */

/* used for qsort function */
int compareCounter(const void * a, const void * b);

/* used for comparing nodes in the generate tree function */
int compareCounterTree(counter first, counter second);