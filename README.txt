Peyton Gardipee and Tushar Verma
Computer Science 537- Operating Systems
Program #4


This program provides wrapper functions for malloc(), realloc(), and free(). Additionally, it provides a function 
called memcheck537 which operates similarly to valgrind's "memcheck" function. These 4 functions combined provide a 
way to verify that the memory management provided by the operating system is behaving correctly. If any mistakes were 
to be made, specifically in managing the heap, one of these 4 functions would catch it, display a warning message, and 
likely exit from program execution.

The program is broken up into three main files: 537malloc.c, rb_tree.c, range_tree.c. Here are the purposes of each:

537malloc.c:
	Create the interface between the user and the program to provide the 4 memory management functions. A "golden model" of 
	what the allocated memory should reflect is kept using a red-black tree. When malloc(), realloc(), or free() is called, 
	the respective changes are made in the red-black tree.

range_tree.c:
	This module creates the interface between 537malloc.c and rb_tree.c. From the perspective of 537malloc.c, any 
	insert/remove/modification is assumed to be an action on a tree in general. The details of balancing according to the red-black 
	tree semantics are hidden. With this structure, a different tree implementation(ie, avl tree) could be swapped in easily.

rb_tree.c:
	This module defines the details of how a red-black tree, in specific, handles tree operations. This is the file that could be replaced 
	to change implementations to something like an avl tree.


Extra Credit Opportunity:
	Create a function that allows the user to see a list of the places from which malloc537 was called and how many total bytes of memory 
	were allocated from each of those places. This could help to detect memory leaks if called at the end of a program, for example.  

	To use this functionality, call "view_allocations" from any program that uses malloc537.


Red-Black tree credit to source:
	https://github.com/clibs/red-black-tree

	
