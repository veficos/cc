

#ifndef __RBTREE__H__INCLUDE__
#define __RBTREE__H__INCLUDE__


#include <stdbool.h>
#include <stdint.h>


typedef enum rbtree_color_e {
	RBTREE_COLOR_RED,
	RBTREE_COLOR_BLACK
} rbtree_color_t;

typedef struct rbtree_node_s {
	struct rbtree_node_s  *left;
	struct rbtree_node_s  *right;
	struct rbtree_node_s  *parent;
	rbtree_color_t         color;
} rbtree_node_t;

typedef int (*rbtree_compare_pt)(const rbtree_node_t*, const rbtree_node_t*);
typedef bool (*rbtree_magic_pt)(rbtree_node_t*);

typedef struct rbtree_s {
	rbtree_node_t *root;
} rbtree_t;


static
void rbtree_link_node(rbtree_node_t **link, rbtree_node_t *parent, rbtree_node_t *node)
{
	node->left = NULL;
	node->right = NULL;
	node->parent = parent;
	node->color = RBTREE_COLOR_RED;
	*link = node;
}


static
void rbtree_replace(rbtree_t *tree, rbtree_node_t *o,
	rbtree_node_t *n)
{
	rbtree_node_t *parent = o->parent;

	if (parent) {
		if (parent->left == o) {
			parent->left = n;

		} else {
			parent->right = n;
		}

	} else {
		tree->root = n;
	}

	if (o->left) {
		o->left->parent = n;
	}

	if (o->right) {
		o->right->parent = n;
	}

	*n = *o;
}

#define rbtree_init(tree)                                                     \
    do {                                                                      \
        ((tree)->root = nullptr);                                             \
    } while (false)

#define rbtree_is_empty(tree)                                                 \
    (!(tree)->root)

#define rbtree_of(ptr, type, member)                                          \
    ((type*)(((unsigned char*)(ptr)) - (unsigned char*)(&(((type*)0)->member))))

#define rbtree_for_each(tree, iterator)                                       \
    for ((iterator) = rbtree_begin(tree);                                     \
         (iterator);                                                          \
         (iterator) = rbtree_next((iterator)))

#define rbtree_reverse_for_each(tree, iterator)                               \
    for ((iterator) = rbtree_rbegin(tree);                                    \
         (iterator);                                                          \
         (iterator) = rbtree_rnext((iterator)))

#define rbtree_safe_for_each(tree, iterator, next_iterator)                   \
    for ((iterator) = rbtree_begin(tree),                                     \
         (next_iterator) = rbtree_next((iterator));                           \
         (iterator);                                                          \
         (iterator) = (next_iterator),                                        \
         (next_iterator) =                                                    \
         (next_iterator) ? rbtree_next((next_iterator)) : NULL)

#define rbtree_safe_reverse_for_each(tree, iterator, next_iterator)           \
    for ((iterator) = rbtree_rbegin(tree),                                    \
         (next_iterator) = rbtree_rnext((iterator));                          \
         (iterator);                                                          \
         (iterator) = (next_iterator),                                        \
         (next_iterator) =                                                    \
         (next_iterator) ? rbtree_rnext((next_iterator)) : NULL)

#define rbtree_end(tree)     NULL

#define rbtree_rend(tree)    NULL

bool rbtree_insert_if(rbtree_t *tree, rbtree_node_t *node, rbtree_compare_pt compare);
rbtree_node_t* rbtree_update_if(rbtree_t *tree, rbtree_node_t *node, rbtree_compare_pt compare);
rbtree_node_t* rbtree_remove_if(rbtree_t *tree, rbtree_node_t *node, rbtree_compare_pt compare);
rbtree_node_t* rbtree_search_if(rbtree_t *tree, rbtree_node_t *node, rbtree_compare_pt compare);
bool rbtree_is_same_tree(rbtree_node_t *lhs, rbtree_node_t *rhs, rbtree_compare_pt compare);
void rbtree_insert(rbtree_t *tree, rbtree_node_t *node);
void rbtree_remove(rbtree_t *tree, rbtree_node_t *node);
void rbtree_clear(rbtree_t *tree, rbtree_magic_pt magic);
void rbtree_magic(rbtree_t *tree, rbtree_magic_pt magic);
rbtree_node_t* rbtree_begin(rbtree_t *tree);
rbtree_node_t* rbtree_rbegin(rbtree_t *tree);
rbtree_node_t* rbtree_next(rbtree_node_t *node);
rbtree_node_t* rbtree_rnext(rbtree_node_t *node);

#endif
