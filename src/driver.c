#include "rbtree.h"
#include "stdio.h"
#include "assert.h"


int main(int argc, char *argv[]) {
    key_t key = 1;
    rbtree *t = new_rbtree();
    node_t *p = rbtree_insert(t, 1);
    node_t *r = rbtree_find(t, key);

    printf("%d\n", r->key);
    assert(r->key == key);
    delete_rbtree(t);
    return 0;
}