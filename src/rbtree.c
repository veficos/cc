

#include "rbtree.h"


#define rbtree_set_red(node)         ((node)->color = RBTREE_COLOR_RED)
#define rbtree_set_black(node)       ((node)->color = RBTREE_COLOR_BLACK)
#define rbtree_is_red(node)          ((node)->color == RBTREE_COLOR_RED)
#define rbtree_is_black(node)        ((node)->color == RBTREE_COLOR_BLACK)
#define rbtree_copy_color(dst, src)  ((dst)->color = (src)->color)


static void __rbtree_left_rotate__(rbtree_t *tree, rbtree_node_t *node);
static void __rbtree_right_rotate__(rbtree_t *tree, rbtree_node_t *node);


bool rbtree_insert_if(rbtree_t *tree, rbtree_node_t *node, rbtree_compare_pt compare)
{
	rbtree_node_t **link = &tree->root;
	rbtree_node_t  *grandpa = NULL;
	rbtree_node_t  *parent = NULL;

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
	rbtree_set_red(node);
	*link = node;

	while ((parent = node->parent) && rbtree_is_red(parent)) {
		grandpa = parent->parent;

		if (parent == grandpa->left) {
			rbtree_node_t *uncle = grandpa->right;

			if (uncle && rbtree_is_red(uncle)) {
				rbtree_set_black(uncle);
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				node = grandpa;
				continue;

			} else {
				if (node == parent->right) {
					parent = node;
					node = node->parent;
					__rbtree_left_rotate__(tree, node);
				}
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				__rbtree_right_rotate__(tree, grandpa);
				break;
			}

		} else {
			rbtree_node_t *uncle = grandpa->left;

			if (uncle && rbtree_is_red(uncle)) {
				rbtree_set_black(uncle);
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				node = grandpa;
				continue;

			} else {
				if (node == parent->left) {
					parent = node;
					node = node->parent;
					__rbtree_right_rotate__(tree, node);
				}
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				__rbtree_left_rotate__(tree, grandpa);
				break;
			}
		}
	}

	rbtree_set_black(tree->root);
	return true;
}


rbtree_node_t *rbtree_update_if(rbtree_t *tree, rbtree_node_t *node, rbtree_compare_pt compare)
{
	rbtree_node_t **link = &tree->root;
	rbtree_node_t  *grandpa = NULL;
	rbtree_node_t  *parent = NULL;

	while (*link) {
		int compare_value = compare(*link, node);

		if (compare_value == 0) {
			if (*link != node) {
				rbtree_node_t *old = *link;
				rbtree_replace(tree, old, node);
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
	rbtree_set_red(node);
	*link = node;

	while ((parent = node->parent) && rbtree_is_red(parent)) {
		grandpa = parent->parent;

		if (parent == grandpa->left) {
			rbtree_node_t *uncle = grandpa->right;

			if (uncle && rbtree_is_red(uncle)) {
				rbtree_set_black(uncle);
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				node = grandpa;
				continue;

			} else {
				if (node == parent->right) {
					parent = node;
					node = node->parent;
					__rbtree_left_rotate__(tree, node);
				}
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				__rbtree_right_rotate__(tree, grandpa);
				break;
			}

		} else {
			rbtree_node_t *uncle = grandpa->left;

			if (uncle && rbtree_is_red(uncle)) {
				rbtree_set_black(uncle);
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				node = grandpa;
				continue;

			} else {
				if (node == parent->left) {
					parent = node;
					node = node->parent;
					__rbtree_right_rotate__(tree, node);
				}
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				__rbtree_left_rotate__(tree, grandpa);
				break;
			}
		}
	}

	rbtree_set_black(tree->root);
	return NULL;
}


rbtree_node_t *rbtree_remove_if(rbtree_t *tree, rbtree_node_t *node, rbtree_compare_pt compare)
{
	rbtree_node_t  *current = tree->root;
	rbtree_node_t  *parent;
	rbtree_node_t  *child;
	rbtree_node_t  *old;
	rbtree_color_t  color;

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
		color = current->color;

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

		current->left = old->left;
		old->left->parent = current;
		current->parent = old->parent;
		rbtree_copy_color(current, old);
		goto maintain;
	}

	parent = current->parent;
	color = current->color;

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
	if (color == RBTREE_COLOR_RED) {
		return old;
	}

	while ((!child || rbtree_is_black(child)) && (child != tree->root)) {
		if (child == parent->left) {
			rbtree_node_t *brother = parent->right;

			if (rbtree_is_red(brother)) {
				rbtree_set_black(brother);
				rbtree_set_red(parent);
				__rbtree_left_rotate__(tree, parent);
				brother = parent->right;
			}

			if ((!brother->left || rbtree_is_black(brother->left)) &&
				(!brother->right || rbtree_is_black(brother->right))) {
				rbtree_set_black(brother);
				child = parent;
				parent = child->parent;

			} else {
				if ((!brother->right || rbtree_is_black(brother->right))) {
					rbtree_set_black(brother->left);
					rbtree_set_red(brother);
					__rbtree_right_rotate__(tree, brother);
					brother = parent->right;
				}

				rbtree_copy_color(brother, parent);
				rbtree_set_black(parent);
				rbtree_set_black(brother->right);
				__rbtree_left_rotate__(tree, parent);
				child = tree->root;
				break;
			}

		} else {
			rbtree_node_t *brother = parent->left;

			if (rbtree_is_red(brother)) {
				rbtree_set_black(brother);
				rbtree_set_red(parent);
				__rbtree_right_rotate__(tree, parent);
				brother = parent->left;
			}

			if ((!brother->left || rbtree_is_black(brother->left)) &&
				(!brother->right || rbtree_is_black(brother->right))) {
				rbtree_set_red(brother);
				child = parent;
				parent = child->parent;

			} else {
				if (!brother->left || rbtree_is_black(brother->left)) {
					rbtree_set_black(brother->right);
					rbtree_set_red(brother);
					__rbtree_left_rotate__(tree, brother);
					brother = parent->left;
				}

				rbtree_copy_color(brother, parent);
				rbtree_set_black(parent);
				rbtree_set_black(brother->left);
				__rbtree_right_rotate__(tree, parent);
				child = tree->root;
				break;
			}
		}
	}

	if (child) {
		rbtree_set_black(child);
	}

	return old;
}


rbtree_node_t *rbtree_search_if(rbtree_t *tree, rbtree_node_t *node, rbtree_compare_pt compare)
{
	rbtree_node_t *current = tree->root;

	while (current) {
		int compare_value = compare(current, node);

		if (compare_value == 0) {
			return current;
		}

		current = compare_value > 0 ? current->left : current->right;
	}

	return NULL;
}


void sb_rbtree_insert(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *parent, *grandpa;

	while ((parent = node->parent) && rbtree_is_red(parent)) {
		grandpa = parent->parent;

		if (parent == grandpa->left) {
			rbtree_node_t *uncle = grandpa->right;

			if (uncle && rbtree_is_red(uncle)) {
				rbtree_set_black(uncle);
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				node = grandpa;
				continue;

			} else {
				if (node == parent->right) {
					parent = node;
					node = node->parent;
					__rbtree_left_rotate__(tree, node);
				}
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				__rbtree_right_rotate__(tree, grandpa);
				break;
			}


		} else {
			rbtree_node_t *uncle = grandpa->left;

			if (uncle && rbtree_is_red(uncle)) {
				rbtree_set_black(uncle);
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				node = grandpa;
				continue;

			} else {
				if (node == parent->left) {
					parent = node;
					node = node->parent;
					__rbtree_right_rotate__(tree, node);
				}
				rbtree_set_black(parent);
				rbtree_set_red(grandpa);
				__rbtree_left_rotate__(tree, grandpa);
				break;
			}
		}
	}

	rbtree_set_black(tree->root);
}


void sb_rbtree_remove(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *child, *parent;
	rbtree_color_t color;

	if (!node->left) {
		child = node->right;

	} else if (!node->right) {
		child = node->left;

	} else {
		rbtree_node_t *old = node;

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

		child = node->right;
		parent = node->parent;
		color = node->color;

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
		rbtree_copy_color(node, old);

		goto maintain;
	}

	parent = node->parent;
	color = node->color;

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
	if (color == RBTREE_COLOR_RED) {
		return;
	}

	while ((!child || rbtree_is_black(child)) && child != tree->root) {
		if (parent->left == child) {
			rbtree_node_t *brother = parent->right;

			if (rbtree_is_red(brother)) {
				rbtree_set_black(brother);
				rbtree_set_red(parent);
				__rbtree_left_rotate__(tree, parent);
				brother = parent->right;
			}

			if ((!brother->left || rbtree_is_black(brother->left)) &&
				(!brother->right || rbtree_is_black(brother->right))) {
				rbtree_set_red(brother);
				child = parent;
				parent = child->parent;

			} else {
				if (!brother->right || rbtree_is_black(brother->right)) {
					rbtree_set_black(brother->left);
					rbtree_set_red(brother);
					__rbtree_right_rotate__(tree, brother);
					brother = parent->right;
				}

				rbtree_copy_color(brother, parent);
				rbtree_set_black(parent);
				rbtree_set_black(brother->right);
				__rbtree_left_rotate__(tree, parent);
				child = tree->root;
				break;
			}

		}
		else {
			rbtree_node_t *brother = parent->left;

			if (rbtree_is_red(brother)) {
				rbtree_set_black(brother);
				rbtree_set_red(parent);
				__rbtree_right_rotate__(tree, parent);
				brother = parent->left;
			}

			if ((!brother->left || rbtree_is_black(brother->left)) &&
				(!brother->right || rbtree_is_black(brother->right))) {
				rbtree_set_red(brother);
				child = parent;
				parent = child->parent;

			} else {
				if (!brother->left || rbtree_is_black(brother->left)) {
					rbtree_set_black(brother->right);
					rbtree_set_red(brother);
					__rbtree_left_rotate__(tree, brother);
					brother = parent->left;
				}

				rbtree_copy_color(brother, parent);
				rbtree_set_black(parent);
				rbtree_set_black(brother->left);
				__rbtree_right_rotate__(tree, parent);
				child = tree->root;
				break;
			}
		}
	}

	if (child) {
		rbtree_set_black(child);
	}
}


/**
* note !!
* ban change key
*/
void rbtree_magic(rbtree_t *tree, rbtree_magic_pt magic)
{
	rbtree_node_t *iterator;

	if (magic) {
		rbtree_for_each(tree, iterator) {
			if (!magic(iterator)) {
				return;
			}
		}
	}
}


void rbtree_clear(rbtree_t *tree, rbtree_magic_pt magic)
{
	rbtree_node_t *iterator, *next_iterator;

	if (tree->root) {
		rbtree_safe_for_each(tree, iterator, next_iterator) {
			sb_rbtree_remove(tree, iterator);

			if (magic) {
				if (!magic(iterator)) {
					return;
				}
			}
		}

		tree->root = NULL;
	}
}


bool rbtree_is_same_tree(rbtree_node_t *lhs, rbtree_node_t *rhs, rbtree_compare_pt compare)
{
	if (!lhs && !rhs)
		return true;

	if (lhs && rhs) {
		if ((compare(lhs, rhs) != 0) ||
			!rbtree_is_same_tree(lhs->left, rhs->left, compare) ||
			!rbtree_is_same_tree(lhs->right, rhs->right, compare)) {
			return false;
		}
		return true;
	}

	return false;
}


rbtree_node_t *rbtree_begin(rbtree_t *tree)
{
	rbtree_node_t *begin = tree->root;

	if (begin == NULL) {
		return NULL;
	}

	while (begin->left) {
		begin = begin->left;
	}

	return begin;
}


rbtree_node_t *rbtree_rbegin(rbtree_t *tree)
{
	rbtree_node_t *rbegin = tree->root;

	if (rbegin == NULL) {
		return NULL;
	}

	while (rbegin->right) {
		rbegin = rbegin->right;
	}

	return rbegin;
}


rbtree_node_t* rbtree_next(rbtree_node_t *node)
{
	rbtree_node_t *parent = NULL;

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


rbtree_node_t *rbtree_rnext(rbtree_node_t *node)
{
	rbtree_node_t *parent = NULL;

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


static void __rbtree_left_rotate__(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *right = node->right;
	rbtree_node_t *parent = node->parent;

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
}


static void __rbtree_right_rotate__(rbtree_t *tree, rbtree_node_t *node)
{
	rbtree_node_t *left = node->left;
	rbtree_node_t *parent = node->parent;

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
}