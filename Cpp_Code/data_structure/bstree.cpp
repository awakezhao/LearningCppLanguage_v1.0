#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ARRAY_LENGTH 20

#if 0

typedef int KEY_VALUE;

struct bstree_node {
    KEY_VALUE key;
    struct bstree_node *left;
    struct bstree_node *right;
};

struct bstree {
    struct bstree_node *root;
};

struct bstree_node *bstree_create_node(KEY_VALUE key) {
    struct bstree_node *node = (struct bstree_node*)malloc(sizeof(struct bstree_node));
    assert(node);

    node->key = key;
    node->left = node->right = NULL;
    return node;
}

int bstree_insert(struct bstree *T, int key) {
    if (T == NULL) return -1;

    if (T->root == NULL) {
        T->root = bstree_create_node(key);
    } else {
        struct bstree_node *parent = T->root;
        struct bstree_node *current = T->root;
        while (current != NULL) {
            parent = current;
            if (key < current->key) current = current->left;
            else if (key > current->key) current = current->right;
            else {
                printf("Exist\n");
                return -1;
            }
        }

        if (key < parent->key)
            parent->left = bstree_create_node(key);
        else 
            parent->right = bstree_create_node(key);
    }

    return 0;
}

int bstree_traversal(struct bstree_node *node) {
    if (node == NULL) return 0;

    bstree_traversal(node->left);
    printf("%4d", node->key);
    bstree_traversal(node->right);
}

#define ARRAY_LENGTH 20

int main()
{
    int keyArray[ARRAY_LENGTH] = {24,25,13,35,23, 26,67,47,38,98, 20,13,17,49,12, 21,9,18,14,15};

    struct bstree T = {0};
    int i = 0;
    for(i = 0; i < ARRAY_LENGTH; i++) {
        bstree_insert(&T, keyArray[i]);
    } 

    bstree_traversal(T.root);
    return 0;
}

#else

typedef int KEY_VALUE;

#define TWO_LINKED_ENTRY(name, type) \
    struct name {                    \
        struct type *left;           \
        struct type *right;          \
    }

struct bstree_node {
    KEY_VALUE data;
    TWO_LINKED_ENTRY(, bstree_node) bst;
};

struct bstree {
    struct bstree_node *root;
};

struct bstree_node *bstree_create_node(KEY_VALUE key) {
    struct bstree_node *node = (struct bstree_node *)malloc(sizeof(struct bstree_node));
    assert(node);

    node->data = key;
    node->bst.left = node->bst.right = NULL;
    return node;
}

int bstree_insert(struct bstree *T, int key) {
    if (T == NULL) return -1;

    if (T->root == NULL) {
        T->root = bstree_create_node(key);
    } else {
        struct bstree_node *parent = T->root;
        struct bstree_node *current = T->root;
        while (current != NULL) {
            parent = current;
            if (key < current->data)
                current = current->bst.left;
            else if (key > current->data)
                current = current->bst.right;
            else {
                return -2;
            }
        }

        if (key < parent->data)
            parent->bst.left = bstree_create_node(key);
        else
            parent->bst.right = bstree_create_node(key);
    }

    return 0;
}

int bstree_traversal(struct bstree_node *node) {
    if (node == NULL) return -1;

    bstree_traversal(node->bst.left);
    printf("%4d", node->data);
    bstree_traversal(node->bst.right);

    return 0;
}

int main() 
{
    int keyArray[ARRAY_LENGTH] = {24,25,13,35,23, 26,67,47,38,98, 20,13,17,49,12, 21,9,18,14,15};

    struct bstree T = {0};
    for(int i = 0; i < ARRAY_LENGTH; i++) {
        bstree_insert(&T, keyArray[i]);
    }

    bstree_traversal(T.root);
    return 0;
}

#endif

