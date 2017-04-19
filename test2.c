#include "mem.h"
#include <stdlib.h>
#include <time.h>

#define OSIZE1 10
#define OSIZE2 150
#define ISIZE 10

int testStandard(){
  printf("Standard test with 2D array size %d x %d:\n", OSIZE1, ISIZE);
  int resInit = Mem_Init(getpagesize());
  int index = 0;
  int** arr = (int**) Mem_Alloc (OSIZE1 * sizeof(int*));
  if (arr == NULL){
    printf("Out of space\n");
    return -1;
  }
  for (int i = 0; i < OSIZE1; i++){
    arr[i] = (int*) Mem_Alloc (ISIZE * sizeof(int));
    if (arr[i] == NULL){
      printf("Out of space at arr[%d]\n", i);
      return -1;
    }
    for (int j = 0; j < ISIZE; j++){
      arr[i][j] = index;
      index++;
    }
  }

  for (int i = 0; i < OSIZE1; i++){
    for (int j = 0; j < ISIZE; j++){
      printf("arr[%d][%d] = %d\n", i, j, arr[i][j]);
    }
  }

  return 0;
}

int testOutOfSpace(){
  printf("Out of space test with 2D array size %d x %d:\n", OSIZE2, ISIZE);
  int resInit = Mem_Init(getpagesize());
  int index = 0;
  int** arr = (int**) Mem_Alloc (OSIZE2 * sizeof(int*));
  if (arr == NULL){
    printf("Out of space\n");
    return -1;
  }
  for (int i = 0; i < OSIZE2; i++){
    arr[i] = (int*) Mem_Alloc (ISIZE * sizeof(int));
    if (arr[i] == NULL){
      printf("Ran out of space at arr[%d]\n", i);
      return -1;
    }
    for (int j = 0; j < ISIZE; j++){
      arr[i][j] = index;
      index++;
    }
  }

  for (int i = 0; i < OSIZE2; i++){
    for (int j = 0; j < ISIZE; j++){
      printf("arr[%d][%d] = %d\n", i, j, arr[i][j]);
    }
  }

  return 0;
}

int main(){
  int std = testStandard();
  printf("\n");
  int oop = testOutOfSpace();
  printf("\n");
  printf("Standard test result: %d\n", std);
  printf("Out of space test result: %d\n", oop);
  if (std == 0 && oop == -1){
    printf("Test case 2: Passed!\n");
    return 0;
  } else {
    printf("Test case 2: Failed!\n");
    return -1;
  }
}
