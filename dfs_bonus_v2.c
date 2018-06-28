//
//  dfs.c
//  CI
//
//  Created by Alek  on 28/10/2017.
//  Copyright © 2017 private. All rights reserved.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct Node; struct Graph; struct List;
void printAllPaths(struct Graph* graph, struct List* currentPath, struct Node* node );

struct List {
    char string[27]; //maximal nr nodes == 27
    struct List* next;
    // the place in the array of the last element
    int placeHolder;
};
struct Graph* createGraph(void);
/* Adds a new Node to the graph at the end */
struct Node* addNode(struct Graph*, char, struct Node* lastNode);
void addEdge(struct Graph*, char*, int length, struct Node* lastNode);
void parseInput(char*, char*, char*);

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
};

int main()
{
    char buffer[50] = {0};
    struct Graph* graph = createGraph();
    struct Node* lastNode = NULL;
    
    while (scanf(" %s", buffer) == 1) {
        char node = '\0', edges[27] = {'\0'};
        sscanf(buffer, "%c-%s", &node, edges);
        int length = (int)strlen(buffer+2);
        lastNode = addNode(graph, node, lastNode);
        addEdge(graph, edges, length, lastNode);
    }
    struct List* liste = (struct List*)malloc(sizeof(struct List));
    liste->next=NULL;
    for (int i = 0; i<27; i++) {
        liste->string[i] = '\0';
    }
    liste->string[0] = graph->firstNode->v;
    graph->firstNode->marked = true;
    if (graph->firstNode != NULL) {
        printAllPaths(graph, liste, graph->firstNode);
    }
    return 0;
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

struct Node* findNode(struct Graph* graph, char v) {
    struct Node* pCrawl = graph->firstNode;
    while (pCrawl) {
        if (pCrawl->v == v)
            return pCrawl;
        pCrawl = pCrawl->next;
    }
    return NULL;
}

void addLast(struct List* liste, struct Node* node) {
    liste->string[(int)strlen(liste->string)] = node->v;
}

void removeLast(struct List* liste) {
    liste->string[(int)strlen(liste->string)-1] = '\0';
}

void printLn(struct List* liste) {
    printf("%s\n", liste->string);
}

bool allNodesInCurrentPath(struct List* liste, struct Graph* graph) {
    struct Node* nodes = graph->firstNode;
    while (nodes) {
        if (strchr(liste->string, nodes->v))
            nodes = nodes->next;
        else
            return 0;
    }
    return 1;
}

bool isLast(struct List* currentPath, struct Graph* graph) {
    if (strlen(currentPath->string)==(graph->nr_Nodes-1)) {
        if (allNodesInCurrentPath(currentPath, graph)) {
            return 1;
        }
    }
    return 0;
}

void printAllPaths(struct Graph* graph, struct List* currentPath, struct Node* node ) {
    char* kids = node->edges;
    for (int i = 0; i<(int)strlen(kids) ; i++) {
        struct Node* myNode = findNode(graph, kids[i]);
        if (isLast(currentPath,graph)) {
            addLast(currentPath, myNode);
            printLn(currentPath);
            removeLast(currentPath);
        }
        else if (!myNode->marked){
            addLast(currentPath, myNode);
            myNode->marked = true;
            printAllPaths(graph, currentPath, myNode);
            myNode->marked = false;
            removeLast(currentPath);
        }
    }
}
