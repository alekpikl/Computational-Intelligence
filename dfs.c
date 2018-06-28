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

struct Node; struct Graph;

// Queue
struct Stack;
struct Stack* createS(void);
void push(struct Stack**, struct Node*);
// returns the first element of the stack. Take the address of the pointer to the stack as arg
struct Node* pop(struct Stack**);
bool isEmpty(struct Stack* stack);
// End of stack
struct Graph* createGraph(void);
/* Adds a new Node to the graph at the end */
struct Node* addNode(struct Graph*, char, struct Node* lastNode);
void addEdge(struct Graph*, char*, int length, struct Node* lastNode);
void parseInput(char*, char*, char*);
void printGraph(struct Graph* graph);
void DFS(struct Graph*);

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
    
    if (graph->firstNode != NULL) {
        DFS(graph);
    }
    return 0;
}

void DFS(struct Graph* graph) {
    struct Stack* stack = createS();
    graph->firstNode->marked = true;
    push(&stack, graph->firstNode);
    //printf("%c",graph->firstNode->v);
    struct Node* popedNode = NULL;
    while (!isEmpty(stack)) {
        popedNode = pop(&stack);
        printf("%c",popedNode->v);
        int i = 0;
        while (popedNode->edges[i] != '\0') {
            // find the NODES one by one
            struct Node* pCrawl = graph->firstNode;
            while (pCrawl->v != popedNode->edges[i]) {
                pCrawl = pCrawl->next;
            }
            // mark & push
            if (!pCrawl->marked) {
                pCrawl->marked = true;
                push(&stack, pCrawl);
            }
            i++;
        }
    }
    printf("\n");
}
// edges from a single node - char array

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

struct Stack {
    struct Node* node;
    struct Stack* next;
};

struct Stack* createS() {
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    stack->next = NULL;
    stack->node = NULL;
    return stack;
}

void push(struct Stack** stack, struct Node* nodeAdd) {
    if ((*stack)->node == NULL) {
        (*stack)->node = nodeAdd;
        (*stack)->next = NULL;
    }
    else {
        struct Stack* newElement = (struct Stack*)malloc(sizeof(struct Stack));
        newElement->node = nodeAdd;
        newElement->next = (*stack);
        (*stack) = newElement;
    }
}

struct Node* pop(struct Stack** stack) {
    struct Node* retHead = (*stack)->node;
    if ((*stack)->next == NULL) {
        (*stack)->next = NULL;
        (*stack)->node = NULL;
    }
    else {
        (*stack) = (*stack)->next;
    }
    return retHead;
}

bool isEmpty(struct Stack* stack) {
    if (stack->node == NULL) return true;
    else return false;
}


