#ifndef RANGE_TREE_H
#define RANGE_TREE_H

//Tree Node Structure
typedef struct node
{
	void *addr;
	size_t length;
	int free_flag;

} node;

typedef struct rb_tree tree;

//Tree Functions
int node_cmp(const void *p1, const void *p2);

void *node_duplicate(void *p);

void node_free(void *p);

tree *tree_create();

void tree_delete(tree *tree);

int node_insert(tree *tree, void *addr, int length);

int tree_erase(tree *tree, void *addr);

node *tree_find(tree *tree, void *addr);

node *tree_find_GLT(tree *tree, void *addr);

int node_isfree(tree *tree, void *addr);

void node_setfree(tree *tree, void *addr, int setVal);

int node_get_length(tree *tree, void *addr);

void tree_print(tree *tree);


#endif