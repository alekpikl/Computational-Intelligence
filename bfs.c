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

struct Node; struct Graph;

// Queue
struct Queue;
struct Queue* createQ();
void enqueue(struct Queue*, struct Node*);
// returns the first element of the queue. Take the address of the pointer to the queue as arg
struct Node* dequeue(struct Queue**);
bool isEmpty(struct Queue* queue);
// End of Queue
struct Graph* createGraph();
/* Adds a new Node to the graph at the end */
struct Node* addNode(struct Graph*, char, struct Node* lastNode);
void addEdge(struct Graph*, char*, int length, struct Node* lastNode);
void parseInput(char*, char*, char*);
void printGraph(struct Graph* graph);
void BDF(struct Graph*);

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
        BDF(graph);
    }
    
    /*
    struct Node* node = NULL;
    struct Queue* queue = createQ();
    printf("Empty? %i\n", isEmpty(queue));
    enqueue(queue, graph->firstNode);
    enqueue(queue, graph->firstNode->next);
    printf("Empty? %i\n", isEmpty(queue));
    enqueue(queue, graph->firstNode->next->next);
    node = dequeue(&queue);
    node = dequeue(&queue);
    node = dequeue(&queue);
    printf("Empty? %i\n", isEmpty(queue));
    enqueue(queue, graph->firstNode);
    
    printGraph(graph);
    */
    
    // Free memory ??
    
    return 0;
}

void BDF(struct Graph* graph) {
    struct Queue* queue = createQ();
    graph->firstNode->marked = true;
    enqueue(queue, graph->firstNode);
    printf("%c",graph->firstNode->v);
    struct Node* dequedNode = NULL;
    while (!isEmpty(queue)) {
        dequedNode = dequeue(&queue);
        int i = 0;
        while (dequedNode->edges[i] != '\0') {
            // find the NODES one by one
            struct Node* pCrawl = graph->firstNode;
            while (pCrawl->v != dequedNode->edges[i]) {
                pCrawl = pCrawl->next;
            }
            // mark & enque
            if (!pCrawl->marked) {
                pCrawl->marked = true;
                enqueue(queue, pCrawl);
                printf("%c",pCrawl->v);
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

struct Queue {
    struct Node* node;
    struct Queue* next;
};

struct Queue* createQ() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->next = NULL;
    queue->node = NULL;
    return  queue;
}

void enqueue(struct Queue* queue, struct Node* nodeAdd) {
    struct Queue* pCrawl = queue;
    if (pCrawl->node == NULL) {
        pCrawl->node = nodeAdd;
        pCrawl->next = NULL;
    }
    else {
        struct Queue* newElement = (struct Queue*)malloc(sizeof(struct Queue));
        newElement->node = nodeAdd;
        newElement->next = NULL;
        while (pCrawl->next) {
            pCrawl = pCrawl->next;
        }
        pCrawl->next = newElement;
    }
}

struct Node* dequeue(struct Queue** queue) {
    struct Node* retHead = (*queue)->node;
    struct Queue* toFree = (*queue);
    if ((*queue)->next == NULL) {
        free((*queue)->next);
        (*queue)->next = NULL;
        (*queue)->node = NULL;
    }
    else {
        free(toFree);
        toFree = NULL;
        (*queue) = (*queue)->next;
    }
    return retHead;
}

bool isEmpty(struct Queue* queue) {
    if (queue->node == NULL) return true;
    else return false;
}

