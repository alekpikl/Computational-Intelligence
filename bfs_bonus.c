//
//  bfs.c
//  CI
//
//  Created by Alek  on 28/10/2017.
//  Copyright © 2017 private. All rights reserved.
//  CREATE GRAPH - OK
//  QUEUE - OK
//  BSF - DO IT!!!

/* Structure:
 Graph -> struct Graph. Members: 1) number of nodes 2) p to the 1st node
 Node -> struct Node. Members: 1) p to the next node 2) p to the char array of with edges from this node
 3) bool if the node has been marked 4) char with the name of the node
 Edge -> char array created at run time.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct Node; struct Graph; struct List;

void permute(char *a, int l, int r, struct List* toCopy, int* nrCycle, char*);

struct Graph* createGraph(void);
/* Adds a new Node to the graph at the end */
struct Node* addNode(struct Graph*, char, struct Node* lastNode);
void addEdge(struct Graph*, char*, int length, struct Node* lastNode);
void parseInput(char*, char*, char*);
void printGraph(struct Graph* graph);
void treeBuild(struct Graph*, int);

void printPermuts(struct Graph* graph, int counter);
int fakultaet(int x);

// The vertices included in the Graph - the arrayList
struct Node {
    int level;
    bool marked;
    char v;
    char* edges;
    struct Node* next;
};

// Vertices ordered alphabetically
struct Graph {
    int nr_Nodes;
    struct Node* firstNode;
};

int main()
{
    char buffer[50] = {0};
    struct Graph* graph = createGraph();
    struct Node* lastNode = NULL;
    int counter = 0;
    
    while (scanf(" %s", buffer) == 1) {
        char node = '\0', edges[27] = {'\0'};
        sscanf(buffer, "%c-%s", &node, edges);
        int length = (int)strlen(buffer+2);
        lastNode = addNode(graph, node, lastNode);
        addEdge(graph, edges, length, lastNode);
        counter++;
    }
    
    if (graph->firstNode != NULL) {
        
        treeBuild(graph, graph->nr_Nodes);
    }
    
    //char arr[3] = {'A', 'B', 'C'};
    //permute(arr, 0, 1, NULL, NULL, NULL);
    
    // Free memory ??
    
    return 0;
}

struct List {
    char string[27]; //maximal nr nodes == 27
    struct List* next;
    // the place in the array of the last element
    int placeHolder;
};
// TEST!!!
void addCopyString(int numberOfCopies, struct List* toCopy, char* toAdd, int nrCycle, char* strToCopy)
{
    // the first time, just add the strToCopy to the end of the first array
    if (nrCycle == 1) {
        strcat(toCopy->string, toAdd);
        // move the PH one forward
        toCopy->placeHolder++;
    }
    else {
        struct List* pCrawl = toCopy;
        // find the end (last element) of the list
        while (pCrawl->next) {
            pCrawl = pCrawl->next;
        }
        for (int i=0; i< numberOfCopies; i++) {
            pCrawl->next = (struct List*)malloc(sizeof(struct List));
            pCrawl->next->placeHolder = toCopy->placeHolder;
            strcpy(pCrawl->next->string, strToCopy);
            if (toAdd) {
                strcat(pCrawl->next->string, toAdd);
            }
            pCrawl = pCrawl->next;
            pCrawl->next = NULL;
        }
    }
}

int isListFull(struct List* liste, int nrNodes) {
    while (liste)
    {
        if (strlen(liste->string) == nrNodes)
            liste = liste->next;
        else
            return 0;
    }
    return 1;
}

char* findChildren(struct Graph* graph, char node, struct List* liste) {
    if (node == '\0') {
        return NULL;
    }
    struct Node* pCrawl = graph->firstNode;
    while (node != pCrawl->v) {
        pCrawl = pCrawl->next;
    }
    char* children = (char *)malloc(sizeof(char) * 27);
    int j=0;
    while (j<27) {
        children[j] = '\0';
        j++;
    }
    // add a feature that recognizes cyrcularity and already marked nodes!!!
    // Go through graph and check
    int i = 0; j = 0;
    char* ret = NULL;
    while (pCrawl->edges[i] != '\0') {
        ret = memchr(liste->string, pCrawl->edges[i], strlen(liste->string));
        if (ret == NULL) {
            children[j] = pCrawl->edges[i];
            j++;
        }
        i++;
    }
    return children;
}

void printList(struct List* liste) {
    while (liste)
    {
        printf("%s\n", liste->string);
        liste = liste->next;
    }
}

void treeBuild(struct Graph* graph, int nrNodes) {
    struct List* liste = (struct List*)malloc(sizeof(struct List));
    memset(liste->string, '\0', 27);
    liste->string[0] = graph->firstNode->v;
    // first PH is 0 -> points to the 1st element
    liste->placeHolder = 0;
    liste->next = NULL;
    // First while ?? ALL THE ARRAYS ARE FULL
    struct List* pCrawl = NULL;
    while (!isListFull(liste, nrNodes)) {
        // ALL THE ARRAYS CHECKED - crawl arrays
        pCrawl = liste; //// WHEN COPYING, FILL OUT THE FIRST ELEMENT AS WELL!!!!
        while (pCrawl) {
            // find children of the array
            // if the PH points to the last element, exit -> array finished
            if (pCrawl->placeHolder >= nrNodes-1) {
                pCrawl = pCrawl->next;
                continue;
            }
            // find the children of the element at the place pointed to by the PH
            char* children = findChildren(graph, pCrawl->string[pCrawl->placeHolder],pCrawl);
            // IF the child already in the string, remove it
            if (*children == '\0') {
                pCrawl->placeHolder++;
                pCrawl = pCrawl->next;
                continue;
            }
            // IF more than 1 child
            if (strlen(children) > 1) {
                // get permutations
                unsigned long int nrChildren = strlen(children);
                // save the array that has to be copied
                char* strToBeCopied = (char*)malloc(sizeof(char)*strlen(pCrawl->string));
                memset(strToBeCopied, '\0', sizeof(*strToBeCopied));
                strcpy(strToBeCopied, pCrawl->string);
                int nrCycles = 0;
                // permute all the children. The first time, add the to the end of the array,
                // all the next times, copy the saved array and add the permuts.
                permute(children, 0, (int)nrChildren-1, pCrawl, &nrCycles, strToBeCopied);
            }
            // ELSE
            else if (strlen(children) == 1){
                // add the child to the array
                strcat(pCrawl->string, children);
                pCrawl->placeHolder++;
            }
            // next placeholder
            pCrawl = pCrawl->next;
        }
        // next array
    }
    printList(liste);
}
// edges from a single node - char array
void swap(char *x, char *y);

/* Function to swap values at two pointers */
void swap(char *x, char *y)
{
    char temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

/* Function to print permutations of string
 This function takes three parameters:
 1. String
 2. Starting index of the string
 3. Ending index of the string. */
void permute(char *a, int l, int r, struct List* liste, int* nrCycle, char* toCopy)
{
    int i;
    if (l == r) {
        //printf("%s\n", a); // HERE COMES THE FUNCTIONSAUFRUF OF THE COPY, ADD
        (*nrCycle)++;
        addCopyString(1, liste, a, *nrCycle, toCopy);
    }
    else
    {
        for (i = l; i <= r; i++)
        {
            swap((a+l), (a+i));
            permute(a, l+1, r, liste, nrCycle, toCopy);
            swap((a+l), (a+i)); //backtrack
        }
    }
}

struct Graph* createGraph() {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->firstNode = NULL;
    graph->nr_Nodes = 0;
    return  graph;
}

struct Node* addNode(struct Graph* graph, char v, struct Node* lastNode) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    
    if (graph->firstNode == NULL) {
        graph->firstNode = newNode;
    }
    else {
        lastNode->next = newNode;
    }
    
    newNode->v = v;
    newNode->edges = NULL;
    newNode->next = NULL;
    newNode->marked = false;
    newNode->level = 0;
    
    graph->nr_Nodes++;
    return newNode;
}

void addEdge(struct Graph* graph, char* destination, int len, struct Node* lastNode) {
    char* edge = (char*)malloc(sizeof(char)*(len+1));
    lastNode->edges = edge;
    
    int i;
    for (i = 0; i < len; i++) {
        edge[i] = destination[i];
    }
    edge[i+1] = '\0';
}

void parseInput(char* buffer, char* edges, char* node) {
    *node = buffer[0];
    if (buffer[2] == '0') {
        return;
    }
    int i = 0;
    do {
        
        i++;
    } while (buffer[i]);
    
}

void printGraph(struct Graph* graph) {
    int v;
    struct Node* pNode = graph->firstNode;
    for (v = 0; v < graph->nr_Nodes; ++v) {
        printf("\n Adjacency list of vertex %c\n head ", pNode->v);
        int i = 0;
        while (pNode->edges[i]!= '\0') {
            printf("-> %c", pNode->edges[i]);
            i++;
        }
        printf("\n");
        pNode = pNode->next;
    }
}



