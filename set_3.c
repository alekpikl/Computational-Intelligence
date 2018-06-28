
//
//  set_3.c
//  CI
//
//  Created by Alek  on 31/01/2018.
//  Copyright © 2018 private. All rights reserved.
//

/*
 Gloabl Vars
 
    world_width = Defines the width of the Grid World
    world_height = Defines the height of the Grid World
    dropOffPenalty = Defines the reward, when falling of a cliff
    stepPenalty = Defines the reward, when moving to another square
    goalReward = Defines the reward, when moving to the goal square
 
 You have to use these global variables, as these variables will be changed by the hidden test-cases.
 
 User - defined Types
 
 Type State: A structure with an x- and y- value that describe the position of the agent. These Values can be accessed via x = s.x and y =  s.y.
 
 Type Action: An enumeration that can be set to UP=0, DOWN=1, LEFT=2, RIGHT=3. These values can be cast to integer by (int) a, where a is of type Action.
 
 Type Agent: A structure with all information about the agent:
 
    type = {SARSA, QLearning} defines the agent type
    epsilon = eps- value of eps- greedy policy
    alpha = the learning rate of the agent
    gamma = the discount factor of the reward
    QValues = 1-dim Array with the respective Q-Values
    accumReward = accumulated reward of a single episode
    avgReward = average accumulated reward over n-epsiodes
 
 All used agents are pointers and hence the members are accessed via
    float eps = agent->eps
    float gamma = agent->gamma
 
 Accessing Q Values of State s and Action a is done via
    float Q_s_a = agent->QValues[stateAction2idx(s, a)]
 *
*/

#include "rl_homework_lib.h"

// Function of the World
State getNextState(State s_t, Action a_t)
{
    switch (a_t) {
        case 0:
            if (s_t.y != 1)
                --s_t.y;
            break;
        case 1:
            if (s_t.y != world_height-2)
                ++s_t.y;
            break;
        case 2:
            if (s_t.x != 1)
                --s_t.x;
            break;
        case 3:
            if (s_t.x != world_width-2)
                ++s_t.x;
            break;
        default:
            break;
    }
    return s_t;
}
float getNextReward(State s_t)
{
    // Cliff
    if ((s_t.y == world_height-2) && (s_t.x >= 2) && (s_t.x <= world_width-3))
        return dropOffPenalty;
    // Goal
    else if (s_t.x == world_width-2 && s_t.y == world_height-2)
        return goalReward;
    else
        return stepPenalty;
    return 0.0;
}
int isTerminalState(State s_t)
{
    if (s_t.x == world_width-2 && s_t.y == world_height-2) {
        return 1;
    } else if ((s_t.y == world_height-2) && (s_t.x >= 2) && (s_t.x <= world_width-3))
        return 1;
    return 0;
}

// Function of the Agent:
Action getNextAction(Agent* a, State s_t)
{
    float rand_value = (double)rand() / (double)RAND_MAX;
    if (rand_value <= a->epsilon)
        return getRandomAction();
    else
        return getGreedyAction(a, s_t);
}
Action getRandomAction()
{
    return rand()%4;
}
Action getGreedyAction(Agent* a, State s_t)
{
    int best_a = 0;
    float best_q = -2000;
    for (int i = 0; i < 4; ++i) {
        if (best_q < a->QValues[stateAction2idx(s_t, i)]) {
            best_q = a->QValues[stateAction2idx(s_t, i)];
            best_a = i;
        }
    }
    return best_a;
}

// Function of Gameplay:
Agent* trainAgent(int nIter, int verbose, Agent* a)
{
    float total_r = 0.0; // BONUS
    for (int episode = 0; episode < nIter; ++episode) {
        playEpisode(episode, verbose, a);
        total_r += a->accumReward;
    }
    a->avgReward = total_r / nIter; // BONUS
    return a;
}

Agent* playEpisode(int nEpisode, int verbose, Agent* a)
{
    // random initial state
    State s_t1;
    s_t1.x = 1;
    s_t1.y = world_height-2;
    a->accumReward = 0.0; // BONUS
    while (!isTerminalState(s_t1)) {
        // initial action - random
        Action rand_action;
        rand_action = getNextAction(a,s_t1);
        
        // Go to that state
        State s_t2 = getNextState(s_t1, rand_action);
        float r_t = getNextReward(s_t2);
        if (a->type == SARSA)
            sarsaUpdate(a, s_t1, s_t2, rand_action, r_t);
        else if (a->type == QLearning)
            qLearningUpdate(a, s_t1, s_t2, rand_action, r_t);
        s_t1 = s_t2;
        a->accumReward += r_t; // BONUS
    }
    
    return a;
}

// SARSA and Q-Learning Updates:

Agent* sarsaUpdate(Agent* a, State s_t1, State s_t2, Action a_t1, float r_t)
{
    Action sarsa_a = getNextAction(a, s_t2);
    a->QValues[stateAction2idx(s_t1, a_t1)] = (1-a->alpha)* a->QValues[stateAction2idx(s_t1, a_t1)] + a->alpha * (r_t + a->gamma * a->QValues[stateAction2idx(s_t2, sarsa_a)]);
    return a;
}
Agent* qLearningUpdate(Agent* a, State s_t1, State s_t2, Action a_t1, float r_t)
{
    // Get best Q - QLearning Update
    Action best_a = getGreedyAction(a, s_t2);
    a->QValues[stateAction2idx(s_t1, a_t1)] = (1-a->alpha)* a->QValues[stateAction2idx(s_t1, a_t1)] + a->alpha * (r_t + a->gamma * a->QValues[stateAction2idx(s_t2, best_a)]);
    return a;
}

