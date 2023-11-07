#include "rbtree.h"
#include "assert.h"
#include "stdio.h"


int main(int argc, char *argv[]) {
    rbtree *t = new_rbtree();
    rbtree_insert(t, 36);
    rbtree_insert(t, 25);
    rbtree_insert(t, 990);
    node_t *p = rbtree_find(t, 36);
    rbtree_erase(t, p);
    return 0;
}