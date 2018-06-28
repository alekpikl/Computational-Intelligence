//
//  bubble_sort.c
//  CI
//
//  Created by Alek  on 28/10/2017.
//  Copyright © 2017 private. All rights reserved.
//

#include <stdio.h>
#include <string.h>

void insertionSort(int* , int);
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
        //printInt(nums, length+2);
        if (length==1) {
            printf("%c\n",string[0]);
        }
        else{
        insertionSort(nums, length);
        }
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

void insertionSort(int* str, int len) {
    int sorted = 1;
    int toBeSorted;
    
    do {
        toBeSorted = str[sorted];
        int insertTo = sorted;
        while (toBeSorted < str[insertTo-1] && insertTo>=0) {
            insertTo--;
        }
        for (int j = sorted; j>insertTo; j--) {
            int temp = str[j-1];
            str[j-1] = str[j];
            str[j] = temp;
        }
        sorted++;
        printStrInt(str, len);
    } while (sorted < len);
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
