#include "mem.h"
#include <stdlib.h>
#include <time.h>

#define testsize 2000000
#define blocksize 8

int testLarge(){
  int freq = 1000;
  int resInit = Mem_Init((blocksize + 32) * testsize + 32);
  clock_t start = clock();
  
  for (int i = 0; i < testsize; i++){
    //printf("alloc #%d\n", i);
    void* pointer = Mem_Alloc(8);
    if (pointer == NULL){
      printf("out of space\n");
      return -1;
    } else {
      //Mem_Dump();
      if (i % freq == freq-1){
	Mem_Free(pointer, 0);
	//printf("free #%d\n", i);
	//Mem_Dump();
      }
    }
  }

  clock_t end = clock();
  double duration = (double) ((end - start) / CLOCKS_PER_SEC);
  printf("Duration for %d mallocs and 1 free every %d mallocs: %f seconds\n", testsize, freq, duration);
  //Mem_Dump();
  return 0;
}

int main(){
  int resOne = testLarge();
  if (resOne == 0){
    printf("Test case 3: Passed!\n");
  } else {
    printf("Test case 3: Failed!\n");
  }
  return 0;
}
