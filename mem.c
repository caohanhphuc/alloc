 #include "mem.h"

struct Node{
  int magic;
  int regsize;
  int stat;
  struct Node *next;
  struct Node *prev;
};

void *mainmem;
struct Node *holes;
int m_error;
int msize;
int minit;

struct Node* newNode(void *ptr, struct Node* nextNode, struct Node *prevNode, int size, int status){

  struct Node *node = (struct Node *) ptr;

  node->magic = 293065;
  node->regsize = size;
  node->stat = status;
  node->next = nextNode;
  node->prev = prevNode;

  return node;
}

int Mem_Init(int sizeOfRegion){
  if (minit == 293065){
    m_error = E_BAD_ARGS;
    return -1;
  }
  if (sizeOfRegion <= 0){
    m_error = E_BAD_ARGS;
    return -1;
  }
  
  int psize = getpagesize();
  if (sizeOfRegion != (sizeOfRegion / psize) * psize){
    msize = (sizeOfRegion / psize + 1) * psize;
  } else {
    msize = sizeOfRegion;
  }

  int fd = shm_open("sharedmem", O_CREAT | O_TRUNC | O_RDWR, 0666);
  if (fd == -1){
    perror("open");
    return -1;
  }
  ftruncate(fd, msize);
  mainmem = mmap(mainmem, msize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
  if (mainmem == MAP_FAILED){
    m_error = E_BAD_ARGS;
    return -1;
  }

  //printf("Range: %p -> %p\n", mainmem, mainmem+msize);

  minit = 293065;
  holes = newNode(mainmem+24, NULL, NULL, msize-24, 0);
  memcpy(mainmem, &m_error, sizeof(int));
  memcpy(mainmem + sizeof(int), &msize, sizeof(int));
  memcpy(mainmem + sizeof(int)*2, &minit, sizeof(int));
  return 0;
}

void *Mem_Alloc(int size){
  if (minit != 293065){
    m_error = E_NO_SPACE;
    return NULL;
  }
  int allocsize;
  if (size != (size / 8) * 8){
    allocsize = (size / 8 + 1) * 8 + 32;
  } else {
    allocsize = size + 32;
  }
  int maxsize = 0;
  struct Node *curWorst = NULL;
  struct Node *temp = holes;

  while (temp != NULL){
    if (temp->magic != 293065){
      m_error = E_PADDING_OVERWRITTEN;
      return NULL;
    }
  
    if (temp->regsize > maxsize){
      maxsize = temp->regsize;
      curWorst = temp;
    }
    temp = temp->next;
  }

  if (maxsize < allocsize){
    m_error = E_NO_SPACE;
    return NULL;
  }

  if (maxsize == (allocsize)){
    curWorst->stat = 1;
    //delete from holes
    if (curWorst == holes){
      holes = curWorst->next;
    } else {
      curWorst->prev->next = curWorst->next;
    }
    return ((void*)curWorst+32);
  } else {
    struct Node *newMem, *newHole;
    struct Node *newHoleNext = curWorst->next;
    struct Node *newHolePrev = curWorst->prev;

    //newMem = newNode((void*)curWorst, (void*)curWorst+allocsize, (void*) curWorst->prev, allocsize, 1);
    if (curWorst->prev != NULL){
      curWorst->prev->next = curWorst+allocsize;
    }
    newMem = newNode((void*) curWorst, NULL, NULL, allocsize, 1);
    if (maxsize - allocsize <= 32){
      m_error = E_NO_SPACE;
      return NULL;
    }
    newHole = newNode((void*)newMem+allocsize, newHoleNext, newHolePrev, maxsize - allocsize, 0);
    
    if (curWorst != holes){
      newHolePrev->next = newHole;
    } else {
      holes = newHole;
    }
   
    return ((void*)newMem + 32);
  }
}

int Mem_Free(void *ptr, int coalesce){
  if (minit != 293065){
    m_error = E_NO_SPACE;
    return -1;
  }
    
  if (ptr == NULL && coalesce == 0){
    m_error = E_BAD_POINTER;
    return -1;
  }

  if (ptr != NULL){
    if (ptr <  mainmem || ptr > (mainmem+msize)){
      m_error = E_BAD_POINTER;
      return -1;
    }

    struct Node *current = (struct Node*) (ptr-32);
    if (current->stat == 1){
      current->stat = 0;

      //add to hole list:
      struct Node *iter = holes;
      while (iter != NULL){
	if (iter->magic != 293065){
	  m_error = E_PADDING_OVERWRITTEN;
	  return -1;
	}
	
	if (iter > current){
	  break;
	}
	iter = iter->next;
      }

      if (iter == holes){
	holes = current;
	current->next = iter;
	iter->prev = current;
      } else {
	current->next = iter;
	current->prev = iter->prev;
	iter->prev->next = current;
	iter->prev = current;
      }
    }
  }

  if (coalesce == 1){
    struct Node *temp = holes;
    struct Node *startc, *endc;
    int totsize = 0;
    int same = 0;
    int done = 0;
    while (temp->next != NULL){
      if (temp->magic != 293065){
	m_error = E_PADDING_OVERWRITTEN;
	return -1;
      }

      if (temp->next == (void*)temp+temp->regsize){
	if (temp->next->next == NULL){
	  done = 1;
	}
	if (same == 0){
	  same = 1;
	  startc = temp;
	  totsize += temp->regsize;
	}
	endc = temp->next;
	totsize += temp->next->regsize;
      } else {
	if (same == 1){
	  done = 1;
	  same = 0;
	}
      }

      if (same == 1 && done == 1){
	struct Node *merged;
	merged = newNode(startc, endc->next, startc->prev, totsize, 0);
	if (startc->prev != NULL){
	  startc->prev->next = merged;
	}
	same = 0;
	done = 0;
      }

      if (temp->next != NULL){
	temp = temp->next;
      } else {
	break;
      }
    }
  } else if (coalesce != 0) {
    return -1;
  }
  
  return 0;
}

void Mem_Dump(){
  printf("\nMEMORY PRINTOUT:\n");
  printf("Memory size: %d\n", msize);
  struct Node *curNext = (struct Node*) (mainmem+24);
  while (curNext != holes){
    printf("Alloc stat: %d, region size: %d\n", curNext->stat, curNext->regsize - 32);
    printf("Node: %p\n", curNext);
    curNext = (struct Node*) ((void*)curNext+curNext->regsize);
  }
  
  struct Node* temp = holes;
  while (temp != NULL){
    printf("Alloc stat: %d, region size: %d\n", temp->stat, temp->regsize - 32);
    printf("Node: %p\n", temp);
    curNext = (struct Node*) ((void*)temp+temp->regsize);
    while (temp->next != curNext && temp->next != NULL){
      printf("Alloc stat: %d, region size: %d\n", curNext->stat, curNext->regsize - 32);
      printf("Node: %p\n", curNext);
      curNext = (struct Node*) ((void*)curNext+curNext->regsize);
    }
    temp = temp->next;
  }

  temp = holes;
  
  /* printf("FREE SPACE ONLY:\n");
  while (temp != NULL){
    printf("Alloc stat: %d, region size: %d\n", temp->stat, temp->regsize - 32);
    printf("Node: %p\n", temp);
    temp = temp->next;
  }
  */
  printf("\n");
  return;
}
