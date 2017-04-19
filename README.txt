Stephanie Cao
CS355, HW5

1. How the program works:
   - Data Structure: one doubly linked list to store all free nodes. The size in each node is the size requested aligned plus 32 bytes for the header. The size printed out with Mem_Dump() is always the actual size. The magic number 293065 is stored at the beginning of every node to avoid headers getting corrupted, which is checked every time a node is visited in alloc or free.
   
   - Mem_Init():     Does not allow multiple inits in one execution
     		     Alloc & free do not work without calling init first
		     
   - Mem_Alloc():    When valid alloc is called, the free list is traversed until the worst fit is found, then create a smaller hole in place of the worst fit to allocate the memory
     		     
   - Mem_Free():     When valid free is called, the free list is traversed to look for the allocated memory to be freed. When coalesce is specified to be 1, the free list is traversed again to check if free nodes are consecutive, if yes merge them.

2. Test cases:
   - Test case 1: Includes several smaller tests for testing:
		     8-byte alignment & worst fit alloc can be checked from printout of the memory
     	       	     odd-sized allocs and free with char*
		     pass bad args to Mem_Init() or call Mem_Init() multiple times
		     initialize and round up to 1 page for size passed to init
		     try to free a NULL pointer: succeed if coalesce = 1, not succeed if coalesce = 0
   - Test case 2: Alloc a 2D int array in size of 1 page, exceeding pagesize, and check if memory can be written to after allocation by printing the 2D array
   - Test case 3: 2 million allocs and occasional frees (can change test size, block size and freeing frequency) with duration

3. No memory leak or errors

4. Other notes:
   - In the two large test cases, I had to allocate more space (totally arbitrarily) for freeing once every 100 mallocs than once every 1000 mallocs, which does not make sense to my implementation, but I haven't figured out why.
   - I keep 2 global variables at the beginning of the memory chunk, allowing 24 bytes for them.
