//
//  rt_homework_lib.h
//  CI
//
//  Created by Alek  on 31/01/2018.
//  Copyright © 2018 private. All rights reserved.
//

#ifndef rl_homework_lib_h
#define rl_homework_lib_h

#ifndef RL_HOMEWORK_LIB_H
#define RL_HOMEWORK_LIB_H

/*
 ##########################################################################################
 Data Types:
 
 - The definitions of the data structures and Prototypes
 - You DO NOT NEED to change any of these data-types
 - You can change these data types for debugging purposes
 
 ##########################################################################################
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>


// ##########################################################################################
// Global Variables (with standard initialization):

extern int     world_width;
extern int     world_height;
extern int     nActions;
extern float   dropOffPenalty;
extern float   stepPenalty;
extern float   goalReward;


typedef enum {UP, DOWN, LEFT, RIGHT} Action;
typedef enum {QLearning, SARSA} AgentType;

typedef struct{
    int x;
    int y;
}State;

typedef struct{
    AgentType type;
    float epsilon;
    float alpha;
    float gamma;
    float accumReward;
    float avgReward;
    float QValues[];
}Agent;

typedef struct{
    const char* key;
    Action value;
}ActionDictionary;



// ##########################################################################################
// All Prototypes:

// Problem 1 World - Prototypes:
State       getNextState(State s_t, Action a_t);
float       getNextReward(State s_t);
int         isTerminalState(State s_t);


// Problem 2 Agent & Gameplay - Prototypes:
Action      getNextAction(Agent* a, State s_t);
Action      getRandomAction();
Action      getGreedyAction(Agent* a, State s_t);

Agent*      playEpisode(int nEpisode, int verbose, Agent* a);
Agent*      trainAgent(int nIter, int verbose, Agent* a);
Agent*      testAgent(int nTests, int verbose, Agent* a);


// Problem 3 Reinforcement Learning - Prototypes:
Agent*      sarsaUpdate(Agent* a, State s_t1, State s_t2, Action a_t1, float r_t);
Agent*      qLearningUpdate(Agent* a, State s_t1, State s_t2, Action a_t1, float r_t);


// Print Function Prototypes:
void        printGridworld(State s_t);
void        printQValues(Agent* a);
void        printPolicy(Agent* a);


// Data Conversion & Initialization Prototypes
const char* action2string(Action a_t);
Action      string2action(char* string);
int         stateAction2idx(State s, Action a);
void        zeroQValues(Agent* a);

// Test Function Prototypes:
void        stateTransitionTest(State s_t1, Action a_t1);
void        terminalSateTest(State s_t);
void        rewardTest(State s_t);

void        randomActionTest(void);
void        greedyActionTest(Agent* a, State s);
void        epsGreedyActionTest(Agent* a, State s);

void        policyTest(Agent* a, State s_t);
void        qValueTest(Agent* a, State s_t, Action a_t);
void        averageRewardTest(Agent* a);
void        SARSAQValueTest(Agent* a);

#endif // RL_HOMEWORK_LIB_H
#endif /* rt_lib_h */
