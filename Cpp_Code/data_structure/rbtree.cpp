#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED 1
#define BLACK 2

typedef int KEY_TYPE;

#define RBTREE_ENTRY(name, type)    \
    struct name {                   \
        struct type *parent;        \
        struct type *left;          \
        struct type *right;         \
        int color;                  \
    }

typedef struct _rbtree_node {
    RBTREE_ENTRY(, _rbtree_node) rbt;
    KEY_TYPE key;
    void *data;
} rbtree_node;

typedef struct _rbtree {
    rbtree_node *root;
    rbtree_node *nil;
} rbtree;

rbtree_node *rbtree_mini(rbtree *T, rbtree_node *x) {

    while (x->rbt.left != T->nil) {
        x = x->rbt.left;
    }
    return x;
}

rbtree_node *rbtree_maxi(rbtree *T, rbtree_node *x) {

    while (x->rbt.right != T->nil) {
        x = x->rbt.right;
    }
    return x;
}

rbtree_node *rbtree_successor(rbtree *T, rbtree_node *x) {

    rbtree_node *y = x->rbt.parent;
    
    if (x->rbt.right != T->nil) {
        return rbtree_mini(T, x);
    }

    while ((y != T->nil) && (x == y->rbt.right)) {
        x = y;
        y = y->rbt.parent;
    }

    return y;
}

void rbtree_left_rotate(rbtree *T, rbtree_node *x) {

    rbtree_node *y = x->rbt.right;

    x->rbt.right = y->rbt.left;
    if (y->rbt.left != T->nil) {
        y->rbt.left->rbt.parent = x;
    }

    y->rbt.parent = x->rbt.parent;
    if (x->rbt.parent == T->nil) {
        T->root = y;
    } else if (x == x->rbt.parent->rbt.left) {
        x->rbt.parent->rbt.left = y;
    } else {
        x->rbt.parent->rbt.right = y;
    }

    y->rbt.left = x;
    x->rbt.parent = y;
}

void rbtree_right_rotate(rbtree *T, rbtree_node *x) {

    rbtree_node *y = x->rbt.left;

    x->rbt.left = y->rbt.right;
    if (y->rbt.right != T->nil) {
        y->rbt.right->rbt.parent = x;
    }

    y->rbt.parent = x->rbt.parent;
    if (x->rbt.parent == T->nil) {
        T->root = y;
    } else if (x == x->rbt.parent->rbt.right) {
        x->rbt.parent->rbt.right = y;
    } else {
        x->rbt.parent->rbt.left = y;
    }

    y->rbt.right = x;
    x->rbt.parent = y;
}

void rbtree_insert_fixup(rbtree *T, rbtree_node *z) {

    rbtree_node *y = T->nil;
    while (z->rbt.parent->rbt.color == RED) {
        if (z->rbt.parent == z->rbt.parent->rbt.parent->rbt.left) {
            y = z->rbt.parent->rbt.parent->rbt.right;

            if (y->rbt.color == RED) {
                z->rbt.parent->rbt.color = y->rbt.color = BLACK;
                z->rbt.parent->rbt.parent->rbt.color = RED;
                z = z->rbt.parent->rbt.parent;
            } else {
                if(z == z->rbt.parent->rbt.right) {
                    z = z->rbt.parent;
                    rbtree_left_rotate(T, z);
                }

                z->rbt.parent->rbt.color = BLACK;
                z->rbt.parent->rbt.parent->rbt.color = RED;
                rbtree_right_rotate(T, z->rbt.parent->rbt.parent);
            }
        } else {
            y = z->rbt.parent->rbt.parent->rbt.left;
            if (y->rbt.color == RED) {
                z->rbt.parent->rbt.color = y->rbt.color = BLACK;
                z->rbt.parent->rbt.parent->rbt.color = RED;
                z = z->rbt.parent->rbt.parent;
            } else {
                if(z == z->rbt.parent->rbt.left) {
                    z = z->rbt.parent;
                    rbtree_right_rotate(T, z);
                }

                z->rbt.parent->rbt.color = BLACK;
                z->rbt.parent->rbt.parent->rbt.color = RED;
                rbtree_left_rotate(T, z->rbt.parent->rbt.parent);
            }
        }
    }

    T->root->rbt.color = BLACK;
}

void rbtree_insert(rbtree *T, rbtree_node *z) {
    rbtree_node *current = T->root;
    rbtree_node *parent = current->rbt.parent;

    while (current != T->nil) {
        parent = current;
        if (z->key < current->key)
            current = current->rbt.left;
        else if (z->key > current->key)
            current = current->rbt.right;
        else {
            return;
        }
    }

    z->rbt.parent = parent;
    if (parent == T->nil) {
        T->root = z;
    } else {
        if (z->key < parent->key)
            parent->rbt.left = z;
        else
            parent->rbt.right = z;
    }

    z->rbt.left = z->rbt.right = T->nil;
    z->rbt.color = RED;

    rbtree_insert_fixup(T, z);
}

//TODO:
rbtree_node *rbtree_delete(rbtree *T, rbtree_node *z) {

    rbtree_node *y = T->nil;
    rbtree_node *x = T->nil;

    if ((z->rbt.left == T->nil) || (z->rbt.right == T->nil)) {
        y = z;
    } else {
        y = rbtree_successor(T, z);
    }


}

rbtree_node *rbtree_search(rbtree *T, KEY_TYPE key) {

    rbtree_node *node = T->root;
    while (node != T->nil) {
        if (key < node->key) {
            node = node->rbt.left;
        } else if (key > node->key) {
            node = node->rbt.right;
        } else {
            return node;
        }
    }
    return T->nil;
}

void rbtree_traversal(rbtree *T, rbtree_node *node) {
    if (node != T->nil) {
        rbtree_traversal(T, node->rbt.left);
        printf("%4d", node->key);
        rbtree_traversal(T, node->rbt.right);
    }
}

int main()
{

    return 0;
}