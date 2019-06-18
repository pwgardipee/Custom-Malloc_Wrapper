#ifndef RB_TREE_H
#define RB_TREE_H

#include <stddef.h>


/* Opaque types */
typedef struct rb_tree rb_tree_t;
typedef struct rb_trav rb_trav_t;

/* User-defined item handling */
typedef int   (*cmp_f) ( const void *p1, const void *p2 );
typedef void *(*dup_f) ( void *p );
typedef void  (*rel_f) ( void *p );

/* Red Black tree functions */
rb_tree_t *rb_new ( cmp_f cmp, dup_f dup, rel_f rel);
void          rb_delete ( rb_tree_t *tree );
void         *rb_find ( rb_tree_t *tree, void *data );
void         *rb_find_GLT(rb_tree_t *tree, void *data, void* curr_high);
int           rb_insert ( rb_tree_t *tree, void *data );
int           rb_erase ( rb_tree_t *tree, void *data );
size_t        rb_size ( rb_tree_t *tree );

/* Traversal functions */
rb_trav_t    *rb_tnew ( void );
void          rb_tdelete ( rb_trav_t *trav );
void         *rb_tfirst ( rb_trav_t *trav, rb_tree_t *tree );
void         *rb_tlast ( rb_trav_t *trav, rb_tree_t *tree );
void         *rb_tnext ( rb_trav_t *trav );
void         *rb_tprev ( rb_trav_t *trav );


#endif
