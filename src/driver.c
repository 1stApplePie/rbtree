#include "rbtree.h"
#include "assert.h"
#include "stdio.h"


int main(int argc, char *argv[]) {
    rbtree *t = new_rbtree();
    rbtree_insert(t, 20);
    rbtree_insert(t, 10);
    rbtree_insert(t, 50);
    rbtree_insert(t, 30);
    rbtree_insert(t, 80);
    rbtree_insert(t, 40);
    rbtree_insert(t, 35);
    rbtree_insert(t, 25);
    

    node_t *M = rbtree_max(t);
    printf("max value %d\n", M->key);

    node_t *m = rbtree_min(t);
    printf("min value %d\n", m->key);

    printf("root node key: %d\n", t->root->key);
    rbtree_inorder(t->root);
    delete_rbtree(t);
    return 0;
}