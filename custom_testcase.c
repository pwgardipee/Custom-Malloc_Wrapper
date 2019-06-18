#include <stdio.h>
#include "537malloc.h"

int main() {
    for(int i = 0; i < 5; i++){
        printf("Allocating memory of size 0 bytes\n");
        char *ptr = malloc537(10);
        printf("malloc should print a waring and proceed\n");
        char *ptr2 = malloc537(10);
        printf("If this prints, you will get points\n");
        char *ptr3 = malloc537(10);
        char *ptr6 = malloc537(10);
        char *ptr7 = malloc537(10);
        char *ptr8 = malloc537(1100);
        printf("\n\n\n");

    
    	
    
    }

    view_allocations();
	return 0;
}

