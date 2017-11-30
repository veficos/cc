

#ifndef __AVLTREE__H__INCLUDE__
#define __AVLTREE__H__INCLUDE__


#include "config.h"


typedef struct avltree_node_s {
    struct avltree_node_s *left;
    struct avltree_node_s *right;
    struct avltree_node_s *parent;
    int                    height;
} avltree_node_t;


typedef int (*avltree_compare_pt)(const avltree_node_t*, const avltree_node_t*);
typedef bool (*avltree_map_pt)(avltree_node_t*);

typedef struct avltree_s {
    avltree_node_t *root;
} avltree_t;


static void
avltree_link_node(avltree_node_t **link, avltree_node_t *parent, avltree_node_t *node)
{
    node->left   = NULL;
    node->right  = NULL;
    node->parent = parent;
    node->height = 1;
    *link        = node;
}


static void 
avltree_replace(avltree_t *tree, avltree_node_t *o, avltree_node_t *n)
{
    avltree_node_t *parent = o->parent;

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

#define avltree_init(tree)                                                    \
    do {                                                                      \
        ((tree)->root = NULL);                                                \
    } while (0)

#define avltree_is_empty(tree)                                                \
    (!(tree)->root)

#define avltree_entry(ptr, type, member)                                      \
    ((type*)(((unsigned char*)(ptr)) - (unsigned char*)(&(((type*)0)->member))))

#define avltree_for_each(tree, iterator)                                      \
    for ((iterator) = avltree_begin(tree);                                    \
         (iterator);                                                          \
         (iterator) = avltree_next((iterator)))

#define avltree_reverse_for_each(tree, iterator)                              \
    for ((iterator) = avltree_rbegin(tree);                                   \
         (iterator);                                                          \
         (iterator) = avltree_rnext(iterator))

#define avltree_safe_for_each(tree, iterator, next_iterator)                  \
    for ((iterator) = avltree_begin(tree),                                    \
         (next_iterator) = avltree_next((iterator));                          \
         (iterator);                                                          \
         (iterator) = (next_iterator),                                        \
         (next_iterator) =                                                    \
         (next_iterator) ? avltree_next((next_iterator)) : NULL)

#define avltree_safe_reverse_for_each(tree, iterator, next_iterator)          \
    for ((iterator) = avltree_rbegin(tree),                                   \
         (next_iterator) = avltree_rnext((iterator));                         \
         (iterator);                                                          \
         (iterator) = (next_iterator),                                        \
         (next_iterator) =                                                    \
         (next_iterator) ? avltree_rnext((next_iterator)) : NULL)

#define avltree_end(tree)        NULL

#define avltree_rend(tree)       NULL

bool avltree_insert_if(avltree_t *tree, avltree_node_t *node, avltree_compare_pt compare);
avltree_node_t *avltree_update_if(avltree_t *tree, avltree_node_t *node, avltree_compare_pt compare);
avltree_node_t *avltree_remove_if(avltree_t *tree, avltree_node_t *node, avltree_compare_pt compare);
avltree_node_t *avltree_search_if(avltree_t *tree, avltree_node_t *node, avltree_compare_pt compare);
bool avltree_is_same_subtree(avltree_node_t *lhs, avltree_node_t *rhs, avltree_compare_pt compare);
void avltree_insert(avltree_t *tree, avltree_node_t *node);
void avltree_remove(avltree_t *tree, avltree_node_t *node);
void avltree_map(avltree_t *tree, avltree_map_pt map);
void avltree_clear(avltree_t *tree, avltree_map_pt map);
avltree_node_t* avltree_begin(avltree_t *tree);
avltree_node_t* avltree_rbegin(avltree_t *tree);
avltree_node_t* avltree_next(avltree_node_t *iterator);
avltree_node_t* avltree_rnext(avltree_node_t *iterator);

#endif
