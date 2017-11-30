

#include "config.h"

#include "avltree.h"


#define avltree_height(node)                                               \
    ((node) ? (node)->height : 0)


#define avltree_max_height(left, right)                                    \
    (avltree_height((left)) > avltree_height((right)) ?                    \
     avltree_height((left)) : avltree_height((right)))


#define avltree_balance(node)                                              \
    (avltree_height((node)->left) - avltree_height((node)->right))


static void __avltree_left_rotate__(avltree_t *tree, avltree_node_t *node);
static void __avltree_right_rotate__(avltree_t *tree, avltree_node_t *node);


bool avltree_insert_if(avltree_t *tree, avltree_node_t *node, avltree_compare_pt compare)
{
    avltree_node_t **link = &tree->root;
    avltree_node_t *parent = NULL;

    while (*link) {
        int compare_value = compare(*link, node);

        if (compare_value == 0) {
            return false;
        }

        parent = *link;
        link = compare_value > 0 ? &(*link)->left : &(*link)->right;
    }

    node->left = NULL;
    node->right = NULL;
    node->parent = parent;
    node->height = 1;
    *link = node;

    while (parent) {
        int balance = avltree_balance(parent);

        parent->height = avltree_max_height(parent->left, parent->right) + 1;

        if (balance > 1) {
            if (avltree_balance(parent->left) > 0) {
                __avltree_right_rotate__(tree, parent);

            } else {
                __avltree_left_rotate__(tree, parent->left);
                __avltree_right_rotate__(tree, parent);
            }

            break;

        } else if (balance < -1) {
            if (avltree_balance(parent->right) < 0) {
                __avltree_left_rotate__(tree, parent);

            } else {
                __avltree_right_rotate__(tree, parent->right);
                __avltree_left_rotate__(tree, parent);
            }

            break;
        }

        parent = parent->parent;
    }

    return true;
}


avltree_node_t* avltree_update_if(avltree_t* tree, avltree_node_t* node, avltree_compare_pt compare)
{
    avltree_node_t **link = &tree->root;
    avltree_node_t *parent = NULL;

    while (*link) {
        int compare_value = compare(*link, node);

        if (compare_value == 0) {
            if (*link != node) {
                avltree_node_t *old = *link;
                avltree_replace(tree, old, node);
                return old;
            }

            return NULL;
        }

        parent = *link;
        link = compare_value > 0 ? &(*link)->left : &(*link)->right;
    }

    node->left = NULL;
    node->right = NULL;
    node->parent = parent;
    node->height = 1;
    *link = node;

    while (parent) {
        int balance = avltree_balance(parent);

        parent->height = avltree_max_height(parent->left, parent->right) + 1;

        if (balance > 1) {
            if (avltree_balance(parent->left) > 0) {
                __avltree_right_rotate__(tree, parent);

            } else {
                __avltree_left_rotate__(tree, parent->left);
                __avltree_right_rotate__(tree, parent);
            }

            break;

        } else if (balance < -1) {
            if (avltree_balance(parent->right) < 0) {
                __avltree_left_rotate__(tree, parent);

            } else {
                __avltree_right_rotate__(tree, parent->right);
                __avltree_left_rotate__(tree, parent);
            }

            break;
        }

        parent = parent->parent;
    }

    return NULL;
}


avltree_node_t* avltree_remove_if(avltree_t *tree, avltree_node_t *node, avltree_compare_pt compare)
{
    avltree_node_t *current = tree->root;
    avltree_node_t *child;
    avltree_node_t *parent;
    avltree_node_t *old;

    while (current) {
        int compare_value = compare(current, node);

        if (compare_value == 0) {
            break;
        }

        current = compare_value > 0 ? current->left : current->right;
    }

    if (current == NULL) {
        return NULL;
    }

    old = current;

    if (!current->left) {
        child = current->right;

    } else if (!current->right) {
        child = current->left;

    } else {
        current = current->right;
        while (current->left) {
            current = current->left;
        }

        if (old->parent) {
            if (old->parent->left == old) {
                old->parent->left = current;

            } else {
                old->parent->right = current;
            }

        } else {
            tree->root = current;
        }

        child = current->right;
        parent = current->parent;

        if (parent == old) {
            parent = current;

        } else {
            if (child) {
                child->parent = parent;
            }
            parent->left = child;

            current->right = old->right;
            old->right->parent = current;
        }

        current->left     = old->left;
        old->left->parent = current;
        current->parent   = old->parent;
        goto maintain;
    }

    parent = current->parent;

    if (child) {
        child->parent = parent;
    }

    if (parent) {
        if (parent->left == current) {
            parent->left = child;

        } else {
            parent->right = child;
        }

    } else {
        tree->root = child;
    }

maintain:
    while (parent) {
        int balance = avltree_balance(parent);

        parent->height = avltree_max_height(parent->left, parent->right) + 1;

        if (balance > 1) {
            if (avltree_balance(parent->left) > 0) {
                __avltree_right_rotate__(tree, parent);

            } else {
                __avltree_left_rotate__(tree, parent->left);
                __avltree_right_rotate__(tree, parent);
            }

        } else if (balance < -1) {
            if (avltree_balance(parent->right) < 0) {
                __avltree_left_rotate__(tree, parent);

            } else {
                __avltree_right_rotate__(tree, parent->right);
                __avltree_left_rotate__(tree, parent);
            }
        }

        parent = parent->parent;
    }

    return old;
}


avltree_node_t* avltree_search_if(avltree_t *tree, avltree_node_t *node, avltree_compare_pt compare)
{
    avltree_node_t *current = tree->root;

    while (current) {
        int compare_value = compare(current, node);

        if (compare_value == 0) {
            return current;
        }

        current = compare_value > 0 ? current->left : current->right;
    }

    return NULL;
}


void avltree_insert(avltree_t *tree, avltree_node_t *node)
{
    while (node) {
        int balance = avltree_balance(node);

        node->height = avltree_max_height(node->left, node->right) + 1;

        if (balance > 1) {
            if (avltree_balance(node->left) > 0) {
                __avltree_right_rotate__(tree, node);

            } else {
                __avltree_left_rotate__(tree, node->left);
                __avltree_right_rotate__(tree, node);
            }

            return ;

        } else if (balance < -1) {
            if (avltree_balance(node->right) < 0) {
                __avltree_left_rotate__(tree, node);

            } else {
                __avltree_right_rotate__(tree, node->right);
                __avltree_left_rotate__(tree, node);
            }

            return;
        }

        node = node->parent;
    }
}


void avltree_remove(avltree_t *tree, avltree_node_t *node)
{
    avltree_node_t *child;
    avltree_node_t *parent;
    avltree_node_t *old;
 
    old = node;

    if (!node->left) {
        child = node->right;

    } else if (!node->right) {
        child = node->left;

    } else {
        node = node->right;
        while (node->left) {
            node = node->left;
        }

        if (old->parent) {
            if (old->parent->left == old) {
                old->parent->left = node;

            } else {
                old->parent->right = node;
            }

        } else {
            tree->root = node;
        }

        child  = node->right;
        parent = node->parent;

        if (parent == old) {
            parent = node;

        } else {
            if (child) {
                child->parent = parent;
            }
            parent->left = child;

            node->right = old->right;
            old->right->parent = node;
        }

        node->left = old->left;
        old->left->parent = node;
        node->parent = old->parent;
        goto maintain;
    }

    parent = node->parent;

    if (child) {
        child->parent = parent;
    }

    if (parent) {
        if (parent->left == node) {
            parent->left = child;

        } else {
            parent->right = child;
        }

    } else {
        tree->root = child;
    }

maintain:
    while (parent) {
        int balance = avltree_balance(parent);

        parent->height = avltree_max_height(parent->left, parent->right) + 1;

        if (balance > 1) {
            if (avltree_balance(parent->left) > 0) {
                __avltree_right_rotate__(tree, parent);

            } else {
                __avltree_left_rotate__(tree, parent->left);
                __avltree_right_rotate__(tree, parent);
            }

        } else if (balance < -1) {
            if (avltree_balance(parent->right) < 0) {
                __avltree_left_rotate__(tree, parent);

            } else {
                __avltree_right_rotate__(tree, parent->right);
                __avltree_left_rotate__(tree, parent);
            }
        }

        parent = parent->parent;
    }
}


void avltree_map(avltree_t *tree, avltree_map_pt map)
{
    avltree_node_t *iterator;

    for (iterator = avltree_begin(tree); iterator; iterator = avltree_next(iterator)) {
        if (!map(iterator)) {
            return ;
        }
    }
}


void avltree_clear(avltree_t *tree, avltree_map_pt map)
{
    avltree_node_t *iterator, *next_iterator;

    if (tree->root) {
        for (iterator = avltree_begin(tree),
             next_iterator = avltree_next(iterator);
             iterator;
             iterator = next_iterator,
             next_iterator = next_iterator ? avltree_next(next_iterator) : NULL) {
            avltree_remove(tree, iterator);
            if (map) {
                if (!map(iterator)) {
                    return ;
                }
            }
        }

        tree->root = NULL;
    }
}


bool avltree_is_same_subtree(avltree_node_t *lhs, avltree_node_t *rhs, avltree_compare_pt compare)
{
    if (!lhs && !rhs)
        return true;

    if (lhs && rhs) {
        if ((compare(lhs, rhs) != 0) ||
            !avltree_is_same_subtree(lhs->left, rhs->left, compare) ||
            !avltree_is_same_subtree(lhs->right, rhs->right, compare)) {
            return false;
        }
        return true;
    }

    return false;
}


avltree_node_t* avltree_begin(avltree_t *tree)
{
    avltree_node_t *begin = tree->root;

    if (begin == NULL) {
        return NULL;
    }

    while (begin->left) {
        begin = begin->left;
    }

    return begin;
}


avltree_node_t* avltree_rbegin(avltree_t *tree)
{
    avltree_node_t *rbegin = tree->root;

    if (rbegin == NULL) {
        return NULL;
    }

    while (rbegin->right) {
        rbegin = rbegin->right;
    }

    return rbegin;
}


avltree_node_t* avltree_next(avltree_node_t *node)
{
    avltree_node_t *parent = NULL;

    if (node->right != NULL) {
        node = node->right;

        while (node->left) {
            node = node->left;
        }

        return node;
    }

    while ((parent = node->parent) && (node == parent->right)) {
        node = parent;
    }

    return parent;
}


avltree_node_t* avltree_rnext(avltree_node_t *node)
{
    avltree_node_t *parent = NULL;

    if (node->left != NULL) {
        node = node->left;

        while (node->right) {
            node = node->right;
        }

        return node;
    }

    while ((parent = node->parent) && (node == parent->left)) {
        node = parent;
    }

    return parent;
}


static void 
__avltree_left_rotate__(avltree_t *tree, avltree_node_t *node)
{
    avltree_node_t *right = node->right;
    avltree_node_t *parent = node->parent;

    if ((node->right = right->left)) {
        right->left->parent = node;
    }

    right->left = node;
    node->parent = right;

    if (parent) {
        if (node == parent->left) {
            parent->left = right;

        } else {
            parent->right = right;
        }

    } else {
        tree->root = right;
    }

    right->parent = parent;

    node->height = avltree_max_height(node->left, node->right) + 1;
    right->height = avltree_max_height(right->left, right->right) + 1;
}


static void
__avltree_right_rotate__(avltree_t *tree, avltree_node_t *node)
{
    avltree_node_t *left = node->left;
    avltree_node_t *parent = node->parent;

    if ((node->left = left->right)) {
        left->right->parent = node;
    }

    left->right = node;
    node->parent = left;

    if (parent) {
        if (node == parent->left) {
            parent->left = left;

        } else {
            parent->right = left;
        }

    } else {
        tree->root = left;
    }

    left->parent = parent;

    node->height = avltree_max_height(node->left, node->right) + 1;
    left->height = avltree_max_height(left->left, left->right) + 1;
}
