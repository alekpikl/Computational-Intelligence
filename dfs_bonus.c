//
//  dfs.c
//  CI
//
//  Created by Alek  on 28/10/2017.
//  Copyright © 2017 private. All rights reserved.
//  CREATE GRAPH - OK
//  QUEUE - OK
//  DFS - DO IT!!!

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
void copyListElement(struct List* to, struct List* from);
struct Node* findNode(struct Graph* graph, char v);

struct List {
    char string[27]; //maximal nr nodes == 27
    struct List* next;
    // the place in the array of the last element
    int placeHolder;
};

void printList(struct List* liste) {
    while (liste)
    {
        printf("%s\n", liste->string);
        liste = liste->next;
    }
}
// End of stack
struct Graph* createGraph(void);
/* Adds a new Node to the graph at the end */
struct Node* addNode(struct Graph*, char, struct Node* lastNode);
void addEdge(struct Graph*, char*, int length, struct Node* lastNode);
void parseInput(char*, char*, char*);
void printGraph(struct Graph* graph);
void swap(char *x, char *y);
void permute(char *a, int l, int r,  char***, int*);

void DFS(struct Graph*, struct Node*, struct List*);

// The vertices included in the Graph - the arrayList
struct Node {
    bool marked;
    char v;
    char* edges;
    struct Node* next;
};

// Vertices ordered alphabetically
struct Graph {
    int nr_Nodes;
    struct Node* firstNode;
    char string[27];
};

char* findChildren(struct Graph* graph, char node, struct List* liste) {
    if (node == '\0') {
        return NULL;
    }
    struct Node* pCrawl = findNode(graph, node);
    
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

struct List* allListsFull(struct List* liste, int nrNodes) {
    while (liste) {
        if (strlen(liste->string) == nrNodes) {
            liste = liste->next;
        }
        else
            return liste;
    }
    return NULL;
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

int main()
{
    char buffer[50] = {0};
    struct Graph* graph = createGraph();
    for (int i = 0 ; i < 27 ; i++) {
        graph->string[i] = '\0';
    }
    struct Node* lastNode = NULL;
    
    while (scanf(" %s", buffer) == 1) {
        char node = '\0', edges[27] = {'\0'};
        sscanf(buffer, "%c-%s", &node, edges);
        int length = (int)strlen(buffer+2);
        lastNode = addNode(graph, node, lastNode);
        addEdge(graph, edges, length, lastNode);
    }
    struct List* liste = (struct List *)malloc(sizeof(struct List));
    liste->next = NULL;
    liste->placeHolder = 1;
    for (int i = 0; i<27; i++) {
        liste->string[i] = '\0';
    }
    liste->string[0] = graph->firstNode->v;
    if (graph->firstNode != NULL) {
        while (!isListFull(liste, graph->nr_Nodes)) {
            DFS(graph, graph->firstNode, liste); // PLACEHOLDER??? 
        }
    }
    printList(liste);
    return 0;
}

struct Node* findNode(struct Graph* graph, char v) {
    struct Node* pCrawl = graph->firstNode;
    while (pCrawl) {
        if (pCrawl->v == v)
            return pCrawl;
        pCrawl = pCrawl->next;
    }
    return NULL;
}

bool nodesInList(struct List* liste, char* node) {
    for (int j = 0 ; j < strlen(node) ; j++) {
        for (int i = 0 ; i < strlen(liste->string) ; i++) {
            if (liste->string[i] == node[j]) {
                return true;
            }
        }
    }
    return false;
}

bool lastNode(struct Graph* graph, char node) {
    struct Node* lastNode = findNode(graph, node);
    if (strlen(lastNode->edges) == 1 )
        return 1;
    return 0;
    // zyklisch??
}

int fakultaetRec(int x) {
    if (x == 1)
        return x;
    return x*fakultaetRec(x-1);
}

void DFS(struct Graph* graph, struct Node* node, struct List* currentList) {
    // IF end reached
    char* kids = findChildren(graph, node->v, currentList);
    kids =  (char*)realloc(kids, strlen(kids));
    if (kids[0] == '\0') {
        currentList->placeHolder++;
        return;
    }
    else {
        unsigned long nrKids = strlen(kids);
        if (nrKids == 1) {
            // IF NOT ALREADY IN THE STRING!!!
            struct Node* foundKid = findNode(graph, kids[0]);
            if (!nodesInList(currentList,&foundKid->v)) {
                currentList->string[(int)strlen(currentList->string)] = foundKid->v;
                if (strlen(currentList->string)<graph->nr_Nodes) {
                    DFS(graph, foundKid, currentList);
                }
            }
        }
        else if(nrKids > 1) {
            // nrKids fakultaet == nr Permuts - allocate memory here
            int nrPermuts = fakultaetRec((int)nrKids);
            
            //struct List* permuts = NULL;
            // find the end of the List????
            //currentList->next = permuts;
            char** listPermuts = (char**)malloc(sizeof(char*)*nrPermuts);
            for (int i=0;i<nrPermuts;i++) {
                listPermuts[i] = (char*)malloc(sizeof(char)*nrKids);
            }
            int counter = 0;
            permute(kids, 0, (int)nrKids-1, &listPermuts, &counter);
            // the first list element will be also filled, hence nrPermits-1
            /* TODO ======= COPYING!!!! EVERYTHING WIRD UEBERSCHRIEBEN */
            struct List* pCrawl = currentList; //currentList points to the "first element" of the List
            struct List**  array = (struct List**)malloc(sizeof(struct List*)*nrPermuts);
            for (int i=0; i<nrPermuts; i++) {
                array[i] = NULL;
            }
            array[0] = currentList;
            // last element of the list
            while (pCrawl->next) {
                pCrawl = pCrawl->next;
            }
            for (int i=0; i<(nrPermuts-1); i++) {
                pCrawl->next = (struct List*)malloc(sizeof(struct List));
                copyListElement(pCrawl->next, currentList);
                array[i+1] = pCrawl->next;
                pCrawl = pCrawl->next;
                pCrawl->next = NULL;
            }
            // Now all the memory is prepared. Call DFS on all the new different strings (permuts)
            // call DFS on all the kids - different permuts
            int j = 0;
            pCrawl = array[0]; // set to the first element
            // go through the whole array of the List Elements that are relevant
            while (j<nrPermuts) {
                for (int i=0;i<nrKids;i++) {
                    struct Node* tempNode = findNode(graph, listPermuts[j][i]);
                    if (!nodesInList(pCrawl, &tempNode->v)) {
                        pCrawl->string[(int)strlen(pCrawl->string)] = tempNode->v;
                        /* MIND THE EDGES!!! IF THEY ALREADY IN THE LIST, DON'T START A NEW SEARCH!!! */
                        if (strlen(pCrawl->string)<graph->nr_Nodes) {
                            DFS(graph, tempNode, pCrawl);
                        }
                    }
                }
                j++;
                pCrawl = array[j]; // j??
            }
            
        }
        free(kids);
    }
    /*
    if (strlen(currentList->string) == graph->nr_Nodes) {
        return;
        }
    else if (node->edges[1] == '\0' && (node != graph->firstNode)) {
        if (!nodesInList(currentList, &node->v)) {
            currentList->string[(int)strlen(currentList->string)] = node->v;
        }
        return;
    } else {
        // Even possible??
        char* kids = findChildren(graph, node->v, currentList);
        if (*kids == '\0') {
            if (!nodesInList(currentList, &node->v)) {
            currentList->string[(int)strlen(currentList->string)]=node->v;
            //currentList->placeHolder++;
            }
            return;
        }
        if (strlen(kids) > 1) {
            unsigned long int nrKids = strlen(kids);
            char* strToBeCopied = (char*)malloc(sizeof(char)*strlen(currentList->string));
            memset(strToBeCopied, '\0', sizeof(*strToBeCopied));
            strcpy(strToBeCopied, currentList->string);
            int nrCycles = 0;
            if (!nodesInList(currentList, kids))
                permute(kids, 0, (int)nrKids-1, currentList, &nrCycles, strToBeCopied, graph);
        }
        if (strlen(kids) == 1) {
            if (!nodesInList(currentList, &kids[0])) {
                currentList->string[(int)strlen(currentList->string)-1] = kids[0];
            }
            return;
        }
        //DFS(graph, node, currentList->next);
        // PLATZHALTER SHOULD POINT TO WHERE THE LAST DFS WAS CALLED!
        // MAKE A BIG WHILE LOOP TO LOOP THROUGH THE NODES UNTILL FULL!
        // THE LENGTH OF THE STRING IS OBTAINED WITH strlen()!!!
     */
}

// edges from a single node - char array
// edges from a single node - char array

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
void permute(char *a, int l, int r, char*** listPermuts, int* counter)
{
    int i;
    if (l == r) {
        //printf("%s\n", a); // HERE COMES THE FUNCTIONSAUFRUF OF THE COPY, ADD
        strcpy((*listPermuts)[*counter], a);
        (*counter)++;
        }
    else
    {
        for (i = l; i <= r; i++)
        {
            swap((a+l), (a+i));
            permute(a, l+1, r,listPermuts, counter);
            swap((a+l), (a+i)); //backtrack
        }
    }
}

// TEST!!!
void CopyString(struct List* to, struct List* from)
{
    for (int i = 0; i<27; i++) {
        to->string[i] = '\0';
    }
    strcpy(to->string, from->string);
}

void copyListElement(struct List* to, struct List* from) {
    memset(to->string, '\0', strlen(to->string));
    strcpy(to->string, from->string);
}

/******** GRAPH METHODS ******/

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
    
    graph->nr_Nodes++;
    return newNode;
}

void addEdge(struct Graph* graph, char* destination, int len, struct Node* lastNode) {
    char* edge = (char*)malloc(sizeof(char)*(len+1));
    for (int i = 0; i<=len; i++) {
        edge[i] = '\0';
    }
    
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
