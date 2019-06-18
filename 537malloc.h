#ifndef MALLOC_H
#define MALLOC_H

#define BUFF_SIZE 1024

void *malloc537(size_t size);

void free537(void *ptr);

void *realloc537(void *ptr, size_t size);

void memcheck537(void *ptr, size_t size);

void view_allocations();


//Structure used to hold list of allocation origin addresses- Extra Credit
typedef struct addr_node{
    void* addr;
    size_t allocated_bytes;
    int num_allocations;
}addr_node;

#endif
