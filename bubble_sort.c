//
//  bubble_sort.c
//  CI
//
//  Created by Alek  on 28/10/2017.
//  Copyright © 2017 private. All rights reserved.
//

#include <stdio.h>
#include <string.h>

void bubbleSort(int* , int);
void c2i(char*, int*, int);
void printStr(char*, int);
void printInt(int*, int);
void printStrInt(int* str, int len);

int main(int argc, const char * argv[]) {
    char string[100];
    int nums[100];
    while (scanf(" %s",string) == 1) {
        int length = strlen(string);
        c2i(string, nums, length);
        //printInt(nums, length);
        bubbleSort(nums, length);
        //printInt(nums, length);
    }
    return 0;
}

void c2i(char* letters, int* numbers, int len)
{
    for (int i = 0 ; i < len ; i++) {
        numbers[i] = letters[i];
    }
}

void bubbleSort(int* str, int len) {
    int i = 0;
    int noSwitch = 0;
    int nrPasses = 0;
    do {
        noSwitch = 1;
        for (i=0; i <= len; i++) {
            if (str[i] > str[i+1] && i < len-1)
            {
                int temp = str[i+1];
                str[i+1] = str[i];
                str[i] = temp;
                noSwitch = 0;
            }
        }
        if(!noSwitch)
        {
            printStrInt(str, len);
        }
        nrPasses++;
    } while (!noSwitch);
    if(noSwitch == nrPasses)
    {
        printStrInt(str, len);
    }
    nrPasses--;
    //printf("Nr of Passes: %d\n",nrPasses);
}

void printStrInt(int* str, int len) {
    //printf("The string: ");
    for (int i = 0 ; i < len ; i++)
    {
        printf("%c", str[i]);
    }
    printf("\n");
}

void printStr(char* str, int len) {
    //printf("The string: ");
    for (int i = 0 ; i < len ; i++)
    {
        printf("%c", str[i]);
    }
    printf("\n");
}

void printInt(int* str, int len) {
    //printf("The string: ");
    for (int i = 0 ; i < len ; i++)
    {
        printf("%d ", str[i]);
    }
    printf("\n");
}
