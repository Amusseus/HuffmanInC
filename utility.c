#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utility.h"
#define SIZE 4096

/* functions for the count struct */

/* creates a counter for given char with count 1 */
counter *createCounter(int ch){
    counter *tempCounter = (counter *)malloc(sizeof(counter));
    tempCounter->c = ch;
    tempCounter->count = 1;
    return tempCounter;
}

/* creates a counter for given char with count freq */
counter *createCounterSize(int ch, int freq){
    counter *tempCounter = (counter *)malloc(sizeof(counter));
    tempCounter->c = ch;
    tempCounter->count = freq;
    return tempCounter;
}

/* given a pointer to the counter, it will increase count by 1 */
void freqUp(counter *ref){
    ref->count++;
}

/* prints the counter to stdout */
void printCounter(counter *ref){
    printf("this char %d has frequency of %d\n",ref->c,ref->count);
}

/* compares two counters */
int compareCounter(const void * a, const void * b){
    counter *tempA = (counter *)a;
    counter *tempB = (counter *)b;
    
    /* if freuqnecy is same then sort by char order */
    if(tempA->count == tempB->count){
        return tempA->c -tempB->c;
    }
    /* sort by count */
    else{
        return tempA->count - tempB->count;
    }
}

/* compare function for counter used for creating binary tree */
int compareCounterTree(counter first, counter second){
    /* if frequency tie then return then first counter comes before */
    if(first.count == second.count){
        return -1;
    }
    return first.count - second.count;
}

/* functions for linked list */

/* generates linked list from list and listsize
* assumes that listSize is greater than 0 */
node *generateLinked(counter *countList, int listSize){
    int i; /* index to traverse countList */
    /* head of the linked list */
    node *head = (node *)malloc(sizeof(node));
    node *current = head; /* node used to build linked list */

    /* first object in list is head node */
    head->car = (countList[0]);
    head->next = NULL;
    head->prev = NULL;
    head->left = NULL;
    head->right = NULL;

    /* starting from the second object */
    i = 1;
    while(i < listSize){
        /* create a tempNode for counter, whose next node is Null */
        node *tempNode = (node *)malloc(sizeof(node));
        tempNode->car = countList[i];
        tempNode->next = NULL;
        tempNode->prev = current;
        tempNode->left = NULL;
        tempNode->right = NULL;

        /* make the previous node's next node tempNode */
        current->next = tempNode;
        /* make the tempNode the new current node */
        current = tempNode;
        i++;
    }

    /* return the head of the linked list */
    return head;
}

/* inserts a node into linkedlist in order(ascending), takes a pointer to a 
* pointer in case the the head of the linked list gets changed and the 
* pointer needs to be redirected to the new head */
void insert(node **linkedlist, node *insert){
    /* declaring vars to use */
    int found = 0;
    node *beforeCur;
    node *current = *linkedlist;
    /* if linked list has no values */
    if(current == NULL){
        *linkedlist = insert;
    }
    /* if insert comes before the head */
    else if(compareCounterTree(insert->car,current->car) < 0){
        insert->next = current;
        current->prev = insert;
        *linkedlist = insert;
    }
    else{
        current = current->next;
        /* if insert comes between two nodes */
        while(current != NULL && !found){
            if(compareCounterTree(insert->car,current->car) < 0){
                /* add node before current node */
                node *temp = current->prev;
                temp->next = insert;
                insert->prev = temp;
                insert->next = current;
                current->prev = insert;
                found = 1;
            }   
            beforeCur = current;
            current = current->next;
        }
        /* if insert comes at the end of linked list */
        if(!found){
            beforeCur->next = insert;
            insert->prev = beforeCur;
        }
    
    }
}

/* returns the node removed from the front
* takes pointer to pointer, since after removing the head, 
* the head needs to be redirected to the next node */
node *takeOut(node **linkedlist){
    node *tempNode;
    node *returnNode = *linkedlist;
    /* if no more nodes in the linked list left */
    if(returnNode->next == NULL){
        returnNode = *linkedlist;
        *linkedlist = NULL;
    }
    else{
        returnNode = *linkedlist;
        tempNode = returnNode->next;
        returnNode->next = NULL;
        tempNode->prev = NULL;
        *linkedlist = tempNode;
    }
    return returnNode;
}

/* assuming given linked list has size 1 or greater */
node *generateTree(node **linkedlist, int size_linked){
    /* declares vars to use */
    int count;
    node *leafOne;
    node *leafTwo;
    count = size_linked;
    /* loop till only one node remains in the list */
    while(count > 1){
        /* mallocs a node */
        node *treeNode = (node *)malloc(sizeof(node));

        /* removes two nodes from the list */
        leafOne = takeOut(linkedlist);
        leafTwo = takeOut(linkedlist);

        /* initialize the values of treeNode */
        treeNode->next = NULL;
        treeNode->prev = NULL;
        treeNode->left = leafOne;
        treeNode->right = leafTwo;
        treeNode->car = *(createCounter(-1));
        treeNode->car.count = (leafOne->car.count) + (leafTwo->car.count);

        /* inserts treeNode back into linked list */
        insert(linkedlist,treeNode);
        count--;
    }
    return *linkedlist;
}

/* prints Linked list/ Used for testing */
void printLinkedList(node *head){
    node *current = head;
    while(current != NULL){
        printCounter(&(current->car));
        current = current->next;
    }
}

/* prints binary tree/ Used for testing */
void printInorder(node *tree){

    /* if node is null, meaning the end has 
    * been reached then go back */
    if(tree == NULL){
        return;
    }

    /* print the left side */
    printInorder(tree->left);

    /* print the current node if the character isn't pointing to NULL */
    if(tree->car.c != -1){
        printCounter(&(tree->car));
    }

    printInorder(tree->right);


}

/* returns 1 if true, 0 if false */
int nodeIsLeaf(node *tree){
    if((tree->left) == NULL && (tree->right) == NULL ){
        return 1;
    }
    return 0;
}

node *getNextNode(node *current, char opt){
    /* if opt 1 then go right */
    if(opt){
        return current->right;
    }
    /* if opt is 0 then go left */
    else{
        return current->left;
    }
}

void getAllHuffCodes(node *tree, char **listCodes, char *huffCode, int count){
    /* if leaf is found then put code into listcodes*/
    if( tree != NULL && nodeIsLeaf(tree)){
        char *tempstr;
        huffCode[count] = '\0';
        tempstr = (char *)malloc((strlen(huffCode)+1) *sizeof(char));
        strcpy(tempstr,huffCode);
        listCodes[tree->car.c] = tempstr; 
        return;
    }
    /* add 0 to string and go left */
    if(tree->left != NULL){
        huffCode[count] = '0';
        getAllHuffCodes(tree->left,listCodes,huffCode,count +1);

    }
    /* add 1 to string and go right */
    if(tree->right != NULL){
        huffCode[count] = '1';
        getAllHuffCodes(tree->right,listCodes,huffCode,count +1);

    }
}