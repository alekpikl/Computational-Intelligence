//
//  dijkstra.c
//  CI
//
//  Created by Alek  on 15/11/2017.
//  Copyright © 2017 private. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct Node; struct Graph; struct Costs;

// Queue
struct Queue;
struct Queue* createQ(void);
void enqueue(struct Queue*, struct Node*);
// returns the first element of the queue. Take the address of the pointer to the queue as arg
struct Node* dequeue(struct Queue**);
bool isEmpty(struct Queue* queue);
// End of Queue
struct Graph* createGraph(void);
/* Adds a new Node to the graph at the end */
struct Node* findNode(struct Graph* graph, char v);
struct Node* addNode(struct Graph*, char, struct Node* lastNode);
void addEdge(struct Graph*, char, struct Node* lastNode);
void parseInput(char*, char*, char*);
void printPaths(struct Graph* graph);

void dijkstra(struct Graph* graph, struct Costs* costs);

// The vertices included in the Graph - the arrayList
struct Node {
    bool marked;
    char v;
    char edges[27];
    int distToSource;
    char shortestPath[27];
    struct Node* next;
};

// Vertices ordered alphabetically
struct Graph {
    int nr_Nodes;
    struct Node* firstNode;
};

struct Costs {
    char u;
    char v;
    int dist;
    struct Costs* next;
};

struct Costs* addCosts(char u, char v, int dist, struct Costs* costs, struct Costs* lastElement) {
    struct Costs* ret = (struct Costs*)malloc(sizeof(struct Costs));
    ret->dist = dist;
    ret->u = u;
    ret->v = v;
    ret->next = NULL;
    if (costs == NULL) {
        return ret;
    }
    else {
        lastElement->next = ret;
        return lastElement->next;
    }
}
bool compareChars(char u1, char v1, char u2, char v2) {
    if (u1 == u2)
        if (v1 == v2)
            return 1;
    if (u1 == v2)
        if (v1 == u2)
            return 1;
    
    return 0;
}
int findDist(struct Costs* costs, char v, char u) {
    while (!compareChars(costs->u, costs->v, u, v)) {
        costs = costs->next;
    }
    return costs->dist;
}

int main() {
    char buffer[10] = {0};
    struct Graph* graph = createGraph();
    struct Node* lastNode = NULL;
    struct Costs* costs = NULL;
    struct Costs* lastCost = NULL;
    
    int count = 0;
    
    while (scanf(" %s", buffer) == 1) {
        count++;
        char node = '\0', edge = '\0';
        int dist = 0;
        sscanf(buffer, "%c-%c-%i", &node, &edge, &dist);
        struct Node* finder = findNode(graph, node);
        if (finder == NULL) {
            lastNode = addNode(graph, node, lastNode);
            addEdge(graph, edge, lastNode);
        }
        else {
            addEdge(graph, edge, finder);
        }
        if (count == 1) {
            costs = addCosts(node, edge, dist, costs, lastCost);
            lastCost = costs;
        }
        else
            lastCost = addCosts(node, edge, dist, costs, lastCost);
    }
    
    dijkstra(graph, costs);
    printPaths(graph);
    
    return 0;
}

bool allMarked(struct Graph* graph) {
    struct Node* pCrawl = graph->firstNode;
    while (pCrawl) {
        if (!pCrawl->marked) {
            return 0;
        }
        pCrawl = pCrawl->next;
    }
    return 1;
}

struct Node* smallestDist(struct Graph* graph) {
    int minDist = 1e6;   // MAYBE TOO SMALL??
    struct Node* pCrawl = graph->firstNode;
    struct Node* minDistNode = NULL;
    while (pCrawl) {
        if (pCrawl->distToSource <= minDist && !pCrawl->marked) {
            minDist = pCrawl->distToSource;
            minDistNode = pCrawl;
        }
        pCrawl = pCrawl->next;
    }
    if (pCrawl) {
        minDistNode->marked = true;
    }
    return minDistNode;
}

void copyNewShortest(struct Node* nodeU, struct Node* nodeV) {
    memset(nodeU->shortestPath, '\0', strlen(nodeU->shortestPath));
    strcpy(nodeU->shortestPath, nodeV->shortestPath);
    int i = 0;
    while (nodeU->shortestPath[i] != '\0') {
        i++;
    }
    nodeU->shortestPath[i] = nodeU->v;
}

void dijkstra(struct Graph* graph, struct Costs* costs) {
    graph->firstNode->distToSource = 0;
    graph->firstNode->shortestPath[0] = graph->firstNode->v;
    struct Node* pCrawl = graph->firstNode;
    //
    while (!allMarked(graph)) {
        pCrawl = smallestDist(graph);
        if (pCrawl)
            pCrawl->marked = true;
        // for all the edges
        int i = 0;
        char v = pCrawl->v;
        while (pCrawl->edges[i] != '\0') {
            struct Node* tmp = findNode(graph, pCrawl->edges[i]);
            char u = tmp->v;
            int distV = pCrawl->distToSource;
            int VtoU = findDist(costs, v, u);
            int distU = tmp->distToSource;
            if (distV + VtoU < distU ) {
                tmp->distToSource = distV + VtoU;
                // new shortest path found -> copy the shortest path
                copyNewShortest(tmp, pCrawl);
            }
            i++;
        }
    }
}

struct Graph* createGraph() {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->firstNode = NULL;
    graph->nr_Nodes = 0;
    return  graph;
}

// TEST!!!
struct Node* findNode(struct Graph* graph, char v) {
    struct Node* pCrawl = graph->firstNode;
    while (pCrawl) {
        if (pCrawl->v == v)
            return pCrawl;
        pCrawl = pCrawl->next;
    }
    return NULL;
}

void addEdge(struct Graph* graph, char destination, struct Node* lastNode) {
    int i = 0;
    while (lastNode->edges[i] != '\0') {
        i++;
    }
    lastNode->edges[i] = destination;
    lastNode->edges[i+1] = '\0';
}

struct Node* addNode(struct Graph* graph, char v, struct Node* lastNode) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    
    if (graph->firstNode == NULL) {
        graph->firstNode = newNode;
    }
    else {
    lastNode->next = newNode;
    }
    
    for (int i = 0 ; i < 26 ; i++) {
        newNode->shortestPath[i] = '\0';
    }
    newNode->distToSource = 10000; // MAYBE TOO SMALL??
    newNode->v = v;
    newNode->edges[0] = '\0';
    newNode->next = NULL;
    newNode->marked = false;
    
    graph->nr_Nodes++;
    return newNode;
}

void printPaths(struct Graph* graph) {
    struct Node* pCrawl = graph->firstNode;
    while (pCrawl) {
        int i = (int)strlen(pCrawl->shortestPath)-1;
        while (i >= 0) {
            printf("%c",pCrawl->shortestPath[i]);
            printf("-");
            i--;
        }
        printf("%d\n",pCrawl->distToSource);
        pCrawl = pCrawl->next;
    }
}
