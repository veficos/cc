

#include "config.h"
#include "xmalloc.h"
#include "avltree.h"
#include "cstring.h"
#include "cspool.h"


typedef struct cspool_node_s {
    avltree_node_t link;
    cstring_t cs;
} cspool_node_t;


static inline
bool __cspool_cleanup__(avltree_node_t *node);


void cspool_uninit(cspool_t *cp)
{
    avltree_clear(&cp->tree, __cspool_cleanup__);
    avltree_init(&cp->tree);
}


cstring_t cspool_get(cspool_t *cp, const void *data, size_t n)
{
    avltree_node_t **current = &cp->tree.root, *parent = NULL;
    cspool_node_t *node;

    while (*current) {
        int compare;
        parent = *current;
        node = avltree_entry(parent, cspool_node_t, link);
        compare = memcmp(node->cs, data, n);
        if (compare == 0) {
            return node->cs;
        }
        current = compare < 0 ? &parent->left : &parent->right;
    }

    node = xmalloc(sizeof(cspool_node_t));
    node->cs = cstring_new_n(data, n);
    avltree_link_node(current, parent, &node->link);
    avltree_insert(&cp->tree, &node->link);
    return node->cs;
}


static inline
bool __cspool_cleanup__(avltree_node_t *node)
{
    cspool_node_t *csn;
    csn = avltree_entry(node, cspool_node_t, link);
    cstring_free(csn->cs);
    xfree(csn);
    return true;
}
