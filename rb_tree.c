#include "rb_tree.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef HEIGHT_LIMIT
#define HEIGHT_LIMIT 64       /* Tallest allowable tree */
#endif

typedef struct rb_node
{
  int red;                    /* Color (1=red, 0=black) */
  void *data;                 /* User-defined content */
  struct rb_node *link[2];    /* Left (0) and right (1) links */
} rb_node_t;

struct rb_tree
{
  rb_node_t *root;    /* Top of the tree */
  cmp_f cmp;          /* Compare two items */
  dup_f dup;          /* Clone an item (user-defined) */
  rel_f rel;          /* Destroy an item (user-defined) */
  size_t size;        /* Number of items (user-defined) */
};

struct rb_trav
{
  rb_tree_t *tree;                  /* Paired tree */
  rb_node_t *curr;                    /* Current node */
  rb_node_t *path[HEIGHT_LIMIT];    /* Traversal path */
  size_t top;                       /* Top of stack */
};

/**
  <summary>
  Checks the color of a red black node
  <summary>
  <param name="root">The node to check</param>
  <returns>1 for a red node, 0 for a black node</returns>
  <remarks>For rb_tree.c internal use only</remarks>
*/
static int is_red(rb_node_t *root)
{
  return root != NULL && root->red == 1;
}

/**
  <summary>
  Performs a single red black rotation in the specified direction
  This function assumes that all nodes are valid for a rotation
  <summary>
  <param name="root">The original root to rotate around</param>
  <param name="dir">The direction to rotate (0 = left, 1 = right)</param>
  <returns>The new root ater rotation</returns>
  <remarks>For rb_tree.c internal use only</remarks>
*/
static rb_node_t *rotate_single(rb_node_t *root, int dir)
{

  if (root->link != NULL)
  {
    rb_node_t *save = root->link[!dir];

    root->link[!dir] = save->link[dir];
    save->link[dir] = root;

    root->red = 1;
    save->red = 0;

    return save;
  }

  else
  {
    return NULL;
  }
}

/**
  <summary>
  Performs a double red black rotation in the specified direction
  This function assumes that all nodes are valid for a rotation
  <summary>
  <param name="root">The original root to rotate around</param>
  <param name="dir">The direction to rotate (0 = left, 1 = right)</param>
  <returns>The new root after rotation</returns>
  <remarks>For rb_tree.c internal use only</remarks>
*/
static rb_node_t *rotate_double(rb_node_t *root, int dir)
{
  if (root->link != NULL)
  {
    root->link[!dir] = rotate_single(root->link[!dir], !dir);
  }
  return rotate_single(root, dir);
}

/**
  <summary>
  Creates an initializes a new red black node with a copy of
  the data. This function does not insert the new node into a tree
  <summary>
  <param name="tree">The red black tree this node is being created for</param>
  <param name="data">The data value that will be stored in this node</param>
  <returns>A pointer to the new node</returns>
  <remarks>
  For rb_tree.c internal use only. The data for this node must
  be freed using the same tree's rel function. The returned pointer
  must be freed using C's free function
  </remarks>
*/
static rb_node_t *new_node(rb_tree_t *tree, void *data)
{
  rb_node_t *retNode = (rb_node_t *)malloc(sizeof *retNode);

  if (retNode == NULL)
    return NULL;

  retNode->red = 1;
  retNode->data = tree->dup(data);
  retNode->link[0] = retNode->link[1] = NULL;

  return retNode;
}

/**
  <summary>
  Creates and initializes an empty red black tree with
  user-defined comparison, data copy, and data release operations
  <summary>
  <param name="cmp">User-defined data comparison function</param>
  <param name="dup">User-defined data copy function</param>
  <param name="rel">User-defined data release function</param>
  <returns>A pointer to the new tree</returns>
  <remarks>
  The returned pointer must be released with rb_delete
  </remarks>
*/
rb_tree_t *rb_new(cmp_f cmp, dup_f dup, rel_f rel)
{
  //Allocate space for a new tree
  rb_tree_t *rtn_tree = (rb_tree_t *)malloc(sizeof *rtn_tree);

  if (rtn_tree == NULL)
    return NULL;

  //Initialize all tree attributes
  rtn_tree->root = NULL;
  rtn_tree->cmp = cmp;
  rtn_tree->dup = dup;
  rtn_tree->rel = rel;
  rtn_tree->size = 0;

  return rtn_tree;
}

/**
  <summary>
  Releases a valid red black tree
  <summary>
  <param name="tree">The tree to release</param>
  <remarks>
  The tree must have been created using rb_new
  </remarks>
*/
void rb_delete(rb_tree_t *tree)
{
  rb_node_t *curr = tree->root;
  rb_node_t *save;

  /*
    Rotate away the left links so that
    we can treat this like the destruction
    of a linked list
  */
  while (curr != NULL)
  {
    if (curr->link[0] == NULL)
    {
      /* No left links, just kill the node and move on */
      save = curr->link[1];
      tree->rel(curr->data);
      free(curr);
    }
    else
    {
      /* Rotate away the left link and check again */
      save = curr->link[0];
      curr->link[0] = save->link[1];
      save->link[1] = curr;
    }

    curr = save;
  }

  free(tree);
}

/**
  <summary>
  Search for a copy of the specified
  node data in a red black tree
  <summary>
  <param name="tree">The tree to search</param>
  <param name="data">The data value to search for</param>
  <returns>
  A pointer to the data value stored in the tree,
  or a null pointer if no data could be found
  </returns>
*/
void *rb_find(rb_tree_t *tree, void *data)
{
  rb_node_t *curr = tree->root;

  while (curr != NULL)
  {
    int cmp = tree->cmp(curr->data, data);

    if (cmp == 0)
      break;

    /*
      If the tree supports duplicates, they should be
      chained to the right subtree for this to work
    */
    curr = curr->link[cmp < 0];
  }

  return curr == NULL ? NULL : curr->data;
}

//Returns node of tree that is Greatest Less Than the given address
void *rb_find_GLT(rb_tree_t *tree, void *data, void *curr_high)
{
  rb_node_t *curr = tree->root;
  int found_highest = 0;

  while (curr != NULL)
  {
    if (tree->cmp(curr->data, curr_high) == 1 && (tree->cmp(data, curr->data) == 1))
    {
      curr_high = curr->data;
      found_highest = 1;
    }

    if (tree->cmp(curr->data, data) == 1)
    {
      curr = curr->link[0];
    }
    else
    {
      curr = curr->link[1];
    }
  }

  if (found_highest == 0)
  {
    return NULL;
  }
  else
  {
    return curr_high;
  }
}

/**
  <summary>
  Insert a copy of the user-specified
  data into a red black tree
  <summary>
  <param name="tree">The tree to insert into</param>
  <param name="data">The data value to insert</param>
  <returns>
  1 if the value was inserted successfully,
  0 if the insertion failed for any reason
  </returns>
*/
int rb_insert(rb_tree_t *tree, void *data)
{
  if (tree->root == NULL)
  {
    /*
      We have an empty tree; attach the
      new node directly to the root
    */
    tree->root = new_node(tree, data);

    if (tree->root == NULL)
      return 0;
  }
  else
  {
    rb_node_t head = {0}; /* False tree root */
    rb_node_t *gndParent, *parent;     /* Grandparent & parent */
    rb_node_t *iterator, *itrParent;     /* Iterator & parent */
    int dir = 0, last = 0;

    /* Set up our helpers */
    parent = &head;
    gndParent = iterator = NULL;
    itrParent = parent->link[1] = tree->root;

    /* Search down the tree for a place to insert */
    //for ( ; ; ) {
    while (1)
    {
      if (itrParent == NULL)
      {
        /* Insert a new node at the first null link */
        iterator->link[dir] = itrParent = new_node(tree, data);

        if (itrParent == NULL)
          return 0;
      }
      else if (is_red(itrParent->link[0]) && is_red(itrParent->link[1]))
      {
        /* Simple red violation: color flip */
        itrParent->red = 1;
        itrParent->link[0]->red = 0;
        itrParent->link[1]->red = 0;
      }

      if (is_red(itrParent) && is_red(iterator))
      {
        /* Hard red violation: rotations necessary */
        int dir2 = parent->link[1] == gndParent;

        if (itrParent == iterator->link[last])
          parent->link[dir2] = rotate_single(gndParent, !last);
        else
          parent->link[dir2] = rotate_double(gndParent, !last);
      }

      /*
        Stop working if we inserted a node. This
        check also disallows duplicates in the tree
      */
      if (tree->cmp(itrParent->data, data) == 0)
        break;

      last = dir;
      dir = tree->cmp(itrParent->data, data) < 0;

      /* Move the helpers down */
      if (gndParent != NULL)
        parent = gndParent;

      gndParent = iterator, iterator = itrParent;
      itrParent = itrParent->link[dir];
    }

    /* Update the root (it may be different) */
    tree->root = head.link[1];
  }

  /* Make the root black for simplified logic */
  tree->root->red = 0;
  ++tree->size;

  return 1;
}

/**
  <summary>
  Remove a node from a red black tree
  that matches the user-specified data
  <summary>
  <param name="tree">The tree to remove from</param>
  <param name="data">The data value to search for</param>
  <returns>
  1 if the value was removed successfully,
  0 if the removal failed for any reason
  </returns>
  <remarks>
  The most common failure reason should be
  that the data was not found in the tree
  </remarks>
*/
int rb_erase(rb_tree_t *tree, void *data)
{
  if (tree->root != NULL)
  {
    rb_node_t head = {0}; /* False tree root */
    rb_node_t *curr, *temp1, *temp2; /* Helpers */
    rb_node_t *found = NULL;  /* Found item */
    int dir = 1;

    /* Set up our helpers */
    curr = &head;
    temp2 = temp1 = NULL;
    curr->link[1] = tree->root;

    /*
      Search and push a red node down
      to fix red violations as we go
    */
    while (curr->link[dir] != NULL)
    {
      int last = dir;

      /* Move the helpers down */
      temp2 = temp1, temp1 = curr;
      curr = curr->link[dir];
      dir = tree->cmp(curr->data, data) < 0;

      /*
        Save the node with matching data and keep
        going; we'll do removal tasks at the end
      */
      if (tree->cmp(curr->data, data) == 0)
        found = curr;

      /* Push the red node down with rotations and color flips */
      if (!is_red(curr) && !is_red(curr->link[dir]))
      {
        if (is_red(curr->link[!dir]))
          temp1 = temp1->link[last] = rotate_single(curr, dir);
        else if (!is_red(curr->link[!dir]))
        {
          rb_node_t *s = temp1->link[!last];

          if (s != NULL)
          {
            if (!is_red(s->link[!last]) && !is_red(s->link[last]))
            {
              /* Color flip */
              temp1->red = 0;
              s->red = 1;
              curr->red = 1;
            }
            else
            {
              int dir2 = temp2->link[1] == temp1;

              if (is_red(s->link[last]))
                temp2->link[dir2] = rotate_double(temp1, last);
              else if (is_red(s->link[!last]))
                temp2->link[dir2] = rotate_single(temp1, last);

              /* Ensure correct coloring */
              curr->red = temp2->link[dir2]->red = 1;
              temp2->link[dir2]->link[0]->red = 0;
              temp2->link[dir2]->link[1]->red = 0;
            }
          }
        }
      }
    }

    /* Replace and remove the saved node */
    if (found != NULL)
    {
      tree->rel(found->data);
      found->data = curr->data;
      temp1->link[temp1->link[1] == curr] =
          curr->link[curr->link[0] == NULL];
      free(curr);
    }

    /* Update the root (it may be different) */
    tree->root = head.link[1];

    /* Make the root black for simplified logic */
    if (tree->root != NULL)
      tree->root->red = 0;

    --tree->size;
  }

  return 1;
}

/**
  <summary>
  Gets the number of nodes in a red black tree
  <summary>
  <param name="tree">The tree to calculate a size for</param>
  <returns>The number of nodes in the tree</returns>
*/
size_t rb_size(rb_tree_t *tree)
{
  return tree->size;
}

/**
  <summary>
  Create a new traversal object
  <summary>
  <returns>A pointer to the new object</returns>
  <remarks>
  The traversal object is not initialized until
  rb_tfirst or rb_tlast are called.
  The pointer must be released with rb_tdelete
  </remarks>
*/
rb_trav_t *rb_tnew(void)
{
  return (rb_trav_t *)malloc(sizeof(rb_trav_t));
}

/**
  <summary>
  Release a traversal object
  <summary>
  <param name="trav">The object to release</param>
  <remarks>
  The object must have been created with rb_tnew
  </remarks>
*/
void rb_tdelete(rb_trav_t *trav)
{
  free(trav);
}

/**
  <summary>
  Initialize a traversal object. The user-specified
  direction determines whether to begin traversal at the
  smallest or largest valued node
  <summary>
  <param name="trav">The traversal object to initialize</param>
  <param name="tree">The tree that the object will be attached to</param>
  <param name="dir">
  The direction to traverse (0 = ascending, 1 = descending)
  </param>
  <returns>A pointer to the smallest or largest data value</returns>
  <remarks>For rb_tree.c internal use only</remarks>
*/
static void *start(rb_trav_t *trav, rb_tree_t *tree, int dir)
{
  trav->tree = tree;
  trav->curr = tree->root;
  trav->top = 0;

  /* Save the path for later traversal */
  if (trav->curr != NULL)
  {
    while (trav->curr->link[dir] != NULL)
    {
      trav->path[trav->top++] = trav->curr;
      trav->curr = trav->curr->link[dir];
    }
  }

  return trav->curr == NULL ? NULL : trav->curr->data;
}

/**
  <summary>
  Traverse a red black tree in the user-specified direction
  <summary>
  <param name="trav">The initialized traversal object</param>
  <param name="dir">
  The direction to traverse (0 = ascending, 1 = descending)
  </param>
  <returns>
  A pointer to the next data value in the specified direction
  </returns>
  <remarks>For rb_tree.c internal use only</remarks>
*/
static void *move(rb_trav_t *trav, int dir)
{
  if (trav->curr->link[dir] != NULL)
  {
    /* Continue down this branch */
    trav->path[trav->top++] = trav->curr;
    trav->curr = trav->curr->link[dir];

    while (trav->curr->link[!dir] != NULL)
    {
      trav->path[trav->top++] = trav->curr;
      trav->curr = trav->curr->link[!dir];
    }
  }
  else
  {
    /* Move to the next branch */
    rb_node_t *last;

    do
    {
      if (trav->top == 0)
      {
        trav->curr = NULL;
        break;
      }

      last = trav->curr;
      trav->curr = trav->path[--trav->top];
    } while (last == trav->curr->link[dir]);
  }

  return trav->curr == NULL ? NULL : trav->curr->data;
}

/**
  <summary>
  Initialize a traversal object to the smallest valued node
  <summary>
  <param name="trav">The traversal object to initialize</param>
  <param name="tree">The tree that the object will be attached to</param>
  <returns>A pointer to the smallest data value</returns>
*/
void *rb_tfirst(rb_trav_t *trav, rb_tree_t *tree)
{
  return start(trav, tree, 0); /* Min value */
}

/**
  <summary>
  Initialize a traversal object to the largest valued node
  <summary>
  <param name="trav">The traversal object to initialize</param>
  <param name="tree">The tree that the object will be attached to</param>
  <returns>A pointer to the largest data value</returns>
*/
void *rb_tlast(rb_trav_t *trav, rb_tree_t *tree)
{
  return start(trav, tree, 1); /* Max value */
}

/**
  <summary>
  Traverse to the next value in ascending order
  <summary>
  <param name="trav">The initialized traversal object</param>
  <returns>A pointer to the next value in ascending order</returns>
*/
void *rb_tnext(rb_trav_t *trav)
{
  return move(trav, 1); /* Toward larger items */
}

/**
  <summary>
  Traverse to the next value in descending order
  <summary>
  <param name="trav">The initialized traversal object</param>
  <returns>A pointer to the next value in descending order</returns>
*/
void *rb_tprev(rb_trav_t *trav)
{
  return move(trav, 0); /* Toward smaller items */
}
