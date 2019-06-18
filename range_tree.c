#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rb_tree.h"
#include "range_tree.h"

//Compare two node according to their addresses
//Return 1 if the first argument has a larger address than second argument
//Return -1 if the second argurment has a larger address than the first argument
//Return 0 if both arguments have the same address
int node_cmp(const void *p1, const void *p2)
{
	node *node1, *node2;

	node1 = (node *)p1;
	node2 = (node *)p2;

	if (node1->addr > node2->addr)
		return 1;

	else if (node1->addr < node2->addr)
		return -1;

	return 0;
}

//Create a new node and copy in the passed argument into the new node
//Return the new node
void *node_duplicate(void *p)
{
	void *dup_p;

	dup_p = calloc(1, sizeof(struct node));
	memmove(dup_p, p, sizeof(struct node));

	return dup_p;
}

//Free the memory of the passed in node
void node_free(void *p)
{
	free(p);
}

//Create a new tree structure and return the new tree
tree *tree_create()
{
	rb_tree_t *rbtree;
	rbtree = rb_new(node_cmp, node_duplicate, node_free);

	return rbtree;
}

//Delete the given tree
void tree_delete(tree *tree)
{
	rb_delete(tree);
}


//Given a tree, an address, and a length,
//Insert a new node into the specified tree
//that contains the given address and length attributes
int node_insert(tree *tree, void *addr, int length)
{
	int ret;

	//Create a new node
	node *ins_node;
	ins_node = calloc(1, sizeof(node));

	//Initialize values
	ins_node->addr = addr;
	ins_node->length = length;
	ins_node->free_flag = 0;

	//Insert the node into the tree
	ret = rb_insert(tree, (void *)ins_node);
	if (ret == 0)
	{
		printf("failed to insert the node with mean %p and weight %d\n", addr, length);
		free(ins_node);
		return -1;
	}

	return 0;
}

//Delete the node from the given tree that corresponds to the 
//Given address
int tree_erase(tree *tree, void *addr)
{
	int ret;
	node *erase_node;

	erase_node = calloc(1, sizeof(node));
	erase_node->addr = addr;

	ret = rb_erase(tree, (void *)erase_node);
	if (ret == 0)
	{
		printf("failed to erase the node with mean %p\n", addr);
		free(erase_node);
		return -1;
	}

	return 0;
}

//Find the node in the given tree that corresponds to the given address
node *tree_find(tree *tree, void *addr)
{
	//node_find will be passed and compared
	//rtn_node will be assigned to the returned node
	node *rtn_node, node_find;

	//assign address to be compared
	node_find.addr = addr;

	//Get matching node and return it
	rtn_node = rb_find(tree, &node_find);
	if (!rtn_node)
	{
		return NULL;
	}
	return rtn_node;
}

//Find the node in the given tree that corresponds
//to the greatest address that is less than the given address
//that has already been inserted into the tree
node *tree_find_GLT(tree *tree, void *addr)
{
	node *rtn_node, node_find, *curr_high;

	//Curr_high will be used to keep a running count of the 
	//Highest address for comparison
	curr_high = malloc(sizeof(node));
	curr_high->addr = (void *)0x1;

	//Assign address to be used for comparison. Returned node
	//Must have an address that is less than this address
	node_find.addr = addr;
	rtn_node = rb_find_GLT(tree, &node_find, curr_high);
	if (!rtn_node)
	{
		return NULL;
	}
	return rtn_node;
}

//Return 1 if the address has been freed already
//Return 0 otherwise
int node_isfree(tree *tree, void *addr)
{

	//Traverses the tree and checks if the free_flag of the
	// node at addr is set.
	// 1--> set (node has been freed). 0 --> not freed
	return tree_find(tree, addr)->free_flag == 1;
}

//Set the free_flag to the given value of setVal of the 
//corresonding node to the given address
void node_setfree(tree *tree, void *addr, int setVal)
{

	//Traverses the tree and checks if the free_flag of the
	// node at addr is set.
	// 1--> set (node has been freed). 0 --> not freed
	tree_find(tree, addr)->free_flag = setVal;
}

//Given a tree and address, return the length associated with the given address
int node_get_length(tree *tree, void *addr)
{
	node *rtn_node, node_find;

	//Find the node as with tree_find
	node_find.addr = addr;
	rtn_node = rb_find(tree, &node_find);
	if (!rtn_node)
	{
		return 0;
	}

	//Return the length of the found node
	return rtn_node->length;
}

//Traverse the given tree and print each nodes address and length 
void tree_print(tree *tree)
{
	int index = 0;
	node *rtn_node;

	rb_trav_t *rbtrav;
	rbtrav = rb_tnew();

	rtn_node = rb_tfirst(rbtrav, tree);
	printf("Index: %d Address: %p Length: %ld\n", index, rtn_node->addr, rtn_node->length);

	index++;

	while ((rtn_node = rb_tnext(rbtrav)) != NULL)
	{
		printf("Index: %d Address: %p Length: %ld\n", index, rtn_node->addr, rtn_node->length);
		index++;
	}
}
