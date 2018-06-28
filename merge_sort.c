//
//  bubble_sort.c
//  CI
//
//  Created by Alek  on 28/10/2017.
//  Copyright © 2017 private. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void c2i(char*, int*, int);
void printStr(char*, int);
void printInt(int*, int);
void printStrInt(int* str, int, int);

// params: pointer to start and end. Returns the pointer to a single element
void mergeSort(int*, int, int);
void merge();

int main(int argc, const char * argv[]) {
    char string[100];
    int nums[100];
    while (scanf(" %s",string) == 1) {
        int length = strlen(string);
        c2i(string, nums, length);
        //printInt(nums, length);
        mergeSort(nums, 0, length-1);
        if (length == 1)
        {
            printStr(string, 1);
        }
        //printInt(nums, length);
    }
    return 0;
}

void mergeSort(int arr[], int start, int end) {
    if (end - start < 1) return;
    
    int mid = (end + start)/2;
    mergeSort(arr, start, mid);
    mergeSort(arr, mid+1, end);
    merge(arr, start, mid, end);
    printStrInt(arr, start, end);
    return;
}

void merge(int* A, int start, int mid, int end)
{
    int length = end - start + 1 ;
    int* B = (int*)malloc(sizeof(A[start])*length);
    for (int i = 0 ; i <= end-start ; i++)
    {
        B[i] = A[start+i];
    }
    int i = start, j = mid+1;
    for (int k = start; k <= end; k++) {
        if (i > mid) {
            A[k] = B[j-start]; j++;
        }
        else if (j > end)
        {
            A[k] = B[i-start]; i++;
        }
        else
        {
            if (B[i-start] < B[j-start]) {
                A[k] = B[i-start];
                i++;
            } else {
                A[k] = B[j-start];
                j++;
            }
        }
    }
    
    free(B);
    B = NULL;
}

void c2i(char* letters, int* numbers, int len)
{
    for (int i = 0 ; i < len ; i++) {
        numbers[i] = letters[i];
    }
}

void printStrInt(int* str, int start, int end) {
    //printf("The string: ");
    for (int i = start ; i <= end ; i++)
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
