//
//  problem_2.c
//  CI
//
//  Created by Alek  on 28/10/2017.
//  Copyright © 2017 private. All rights reserved.
//  Multiply ba 2

#include <stdio.h>

int main(int argc, const char * argv[]) {
    int input;
    while (scanf(" %d",&input) == 1) {
        input+=input;
        printf("%d\n",input);
    }
    return 0;
}
