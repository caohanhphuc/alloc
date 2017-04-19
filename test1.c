#include "mem.h"
#include <stdlib.h>
#include <time.h>

char* words[5] = {"word one", "a long long long steph long long word", "a pretty short phrase", "dfs", "bzbzbzbz"};

int testInit(){
  printf("Test Init: Bad args to Mem_Init()\n");
  int initOne = Mem_Init(-1);
  if (initOne == 0){
    printf("Init with size -1 succeeded.\n");
  } else {
    printf("Failed to init with size -1.\n");
  }

  int initTwo = Mem_Init(getpagesize()-100);
  printf("Requesting %d bytes, page size is %d bytes.\n", getpagesize()-100, getpagesize());
  Mem_Dump();
  
  int initThree = Mem_Init(getpagesize());
  if (initThree == 0){
    printf("Init succeeded twice.\n\n");
  } else {
    printf("Failed to call init twice.\n\n");
  }

  if (initOne == -1 && initTwo == 0 && initThree == -1){
    return 0;
  }
  return -1;
}

int testFreeNull(){
  int freep = Mem_Free(NULL, 1);
  if (freep == 0){
    printf("Tried to free NULL pointer, succeeded because coalesce = 1\n");
  } else {
    printf("Could not free NULL pointer.\n");
    return -1;
  }

  freep = Mem_Free(NULL, 0);
  if (freep == -1){
    printf("Tried to free NULL pointer, not succeeded because coalesce = 0\n");
  } else {
    printf("Freed NULL pointer.\n");
    return -1;
  }
  return 0;
}

int testWorstFit(){
  printf("Testing Worst Fit Allocation\n");
  int resInit = Mem_Init(30000 * getpagesize());
  if (resInit == 0){
    printf("Init succeeded twice.\n");
    return -1;
  }
 
  for (int i = 0; i < 16; i++){
    printf("alloc #%d\n", i);
    char* chosen = words[i%5];
    int length = strlen(chosen) + 1;
    char* pointer = (char*)Mem_Alloc(length*sizeof(char));
    if (pointer == NULL){
      printf("out of space\n");
      return -1;
    } else {
      memcpy(pointer, chosen, length);
      Mem_Dump();

      if (i % 3 == 2){
	Mem_Free(pointer, 0);
	printf("free #%d without coalesce\n", i);
	Mem_Dump();
      }
      if (i % 4 == 3){
	Mem_Free(pointer, 1);
	printf("free #%d with coalesce\n", i);
	Mem_Dump();
      }
    }
  }
  return 0;
}

int main(){
  printf("Testing..\n");

  int resInit = testInit();
  int resWorstFit = testWorstFit();
  int resFreeNull = testFreeNull();
  printf("\nresInit: %d\n", resInit);
  printf("resWorstFit: %d\n", resWorstFit);
  printf("resFreeNull: %d\n\n", resFreeNull);

  if (resInit == 0 && resWorstFit == 0 && resFreeNull == 0){
    printf("Test case 1: Passed!\n");
    return 0;
  } else {
    printf("Test case 1: Failed!\n");
    return -1;
  }
}
