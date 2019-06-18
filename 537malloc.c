#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "range_tree.h"
#include "537malloc.h"

//Tree to hold allocations for main program functionality 
static tree *tree_main;

//Variables used to keep track of orgin address allocations for extra credit
static addr_node* addr_arr[BUFF_SIZE];
static int arr_index = 0;

//Extra Credit- This function adds an origin address to the list
void add_addr(void* address, size_t size)
{
	int found = 0;
	
	//Update an existing origin array if found
	for(int i = 0; i < arr_index; i++)
	{
		if(addr_arr[i]->addr == address)
		{
			found = 1;
			addr_arr[i]->allocated_bytes = addr_arr[i]->allocated_bytes + size;
			addr_arr[i]->num_allocations++;
			break;

		}
	}

	//If this is a new origin address, add it to the array
	if(!found)
	{
		addr_arr[arr_index] = malloc(sizeof(addr_node));
		addr_arr[arr_index]->addr = address;
		addr_arr[arr_index]->allocated_bytes = size;
		addr_arr[arr_index]->num_allocations = 1;
		arr_index++;
	}

}


//Extra Credit- This function prints all origin addresses, the number of times that the origin address
//was called, and the total allocation by the origin address
void view_allocations()
{
	for(int i = 0; i < arr_index; i++)
	{
		printf("The instruction at address: %p called malloc537 %d times to allocate a total of %ld bytes of memory\n", addr_arr[i]->addr, addr_arr[i]->num_allocations, addr_arr[i]->allocated_bytes);
	}
	
}

void *malloc537(size_t size)
{

	// int test_sp = 0;
	// for(int i = 0; i < 48; i=i+4){
	// 	printf("Address of tp: %p offset: %d\n", (char*)*((&test_sp)-i), i );
	// }
	// printf("\n\n");
	
	volatile long testarr[2];
	long sneak[1];
	testarr[1] = 12;
	if(testarr[1])
	{

	}

    //feel free to adjust the search limits
    for( int i = -32; i <= 32; ++i) {
        printf("offset %3d: data 0x%08X\n", i, sneak[i]);
		//printf("Address form: %p\n", (char*)sneak[i]);
    }

	if(size == 0) {
		fprintf(stderr, "Warning: Allocating memory of size 0\n");
	}

	//If this is the first malloc, create the tree
	if(tree_main == NULL)
	{
		tree_main = tree_create();
	}

	void* retVal = malloc(size);
	if(retVal == NULL)
	{
		fprintf(stderr, "Malloc failed");
		exit(EXIT_FAILURE);
	}

	//Previous node in tree to the newly allocated memory
	node *prevPtr = tree_find_GLT(tree_main, retVal);

	//Previous node to end of tree, indicating a node exists in the middle
	//of the potentially allocated memory
	node *containPtr = tree_find_GLT(tree_main, (retVal + size));


	//Potentially Split Node

	//If the node previous to the potential node is free and is overlapping
	//The space of the potential node, then split the node
	if(prevPtr != NULL)
	{
		if((prevPtr->free_flag == 1) && ((prevPtr->addr + prevPtr->length) > retVal))
		{
			//Reduce length of free node to be new (pointer - previous pointer)
			prevPtr->length = (retVal - prevPtr->addr);

		}		
	}
	
	//Else if the range of the potential node includes the start of a an existing free node
	else if(containPtr != NULL)
	{
		if((retVal < containPtr->addr) && (containPtr->free_flag == 1))
		{
			//Delete the free node from the tree
			tree_erase(tree_main, prevPtr->addr);
		}
	}

	//Add the allocation to the tree
	node_insert(tree_main, retVal, size);

	// printf("offset %3d: data 0x%08X\n", 3, sneak[3 + sizeof(testarr[1])]);
	// printf("Address form: %p\n", (char*)sneak[]);

	//Add the origin address and allocation size to the list
	add_addr(__builtin_return_address(0), size);
	
	//USE THIS TO INSERT
	//add_addr((char*)sneak[3], size);

	return retVal;
}

void free537(void *ptr) {

	if (ptr == NULL) {
		fprintf(stderr, "Null pointer err\n");
		exit(EXIT_FAILURE);
	}

	//check if ptr points to the first byte 
	// or memory not allocated by 537malloc()
	if (tree_find(tree_main,ptr) == NULL) {
		fprintf(stderr, "Mem not alocated by 537malloc() or bad pointer\n");
		exit(EXIT_FAILURE);
	}

	if (node_isfree(tree_main,ptr)) {
		fprintf(stderr, "Node has already been freed\n");
		exit(EXIT_FAILURE);
	}

	//set the free_flag to 1
	node_setfree(tree_main,ptr,1);
	free(ptr);	
}

void *realloc537(void *ptr, size_t size) {

	if(size == 0) {
		fprintf(stderr, "Warning: Allocating memory of size 0\n");
	}

	if (ptr == NULL) {
		return malloc537(size);
	}

	if ( ptr != NULL && size == 0) {
		free537(ptr);
		return NULL;
		//node_setfree(tree_main,ptr,0); ????
		
	}

	node_setfree(tree_main,ptr,1);
	
	void* rtn_ptr = realloc(ptr, size);

/////

	//Previous node in tree to the newly allocated memory
	node *prevPtr = tree_find_GLT(tree_main, rtn_ptr);

	//Previous node to end of tree, indicating a node exists in the middle
	//of the potentially allocated memory
	node *containPtr = tree_find_GLT(tree_main, (rtn_ptr + size));


	//Potentially Split Node

	//If the node previous to the potential node is free and is overlapping
	//The space of the potential node, then split the node
	if(prevPtr != NULL)
	{
		if((prevPtr->free_flag == 1) && ((prevPtr->addr + prevPtr->length) > rtn_ptr))
		{
			//Reduce length of free node to be new (pointer - previous pointer)
			prevPtr->length = (rtn_ptr - prevPtr->addr);

		}		
	}
	
	//Else if the range of the potential node includes the start of a an existing free node
	else if(containPtr != NULL)
	{
		if((rtn_ptr < containPtr->addr) && (containPtr->free_flag == 1))
		{
			//Delete the free node from the tree
			tree_erase(tree_main, prevPtr->addr);
		}
	}
////////

	node_insert(tree_main,rtn_ptr,size);
	return rtn_ptr;
	

}


void memcheck537(void *ptr, size_t size) {

	if(size == 0) {
		fprintf(stderr, "Warning: Allocating memory of size 0\n");
	}

	if(ptr == NULL) {
		fprintf(stderr, "Invalid address- Pointer is NULL\n");
		exit(EXIT_FAILURE);
	}

	node *nodePtr = tree_find(tree_main,ptr);

	//Node is allocated in the tree
	if(nodePtr != NULL)
	{
		if (size > nodePtr->length) {
			fprintf(stderr, "Memory out of allocated bounds\n");
			exit(EXIT_FAILURE);
		}
	}
	else
	{

		//ptr is potentially in the middle of an allocated block
		//GLT -> finds greatest allocated node that is less than ptr	
		nodePtr = tree_find_GLT(tree_main, ptr);

		if(nodePtr == NULL)
		{
			fprintf(stderr, "Starting address exists before address of first allocated memory adddress\n");
			exit(EXIT_FAILURE);
		}

		if(ptr > (nodePtr->addr + nodePtr->length))
		{
			fprintf(stderr, "Starting address is out of bounds\n");
			exit(EXIT_FAILURE);
		}

		if((ptr + size) > (nodePtr->addr + nodePtr->length))
		{
			fprintf(stderr, "Ending address is out of bounds\n");
			exit(EXIT_FAILURE);
		}

	}

}

