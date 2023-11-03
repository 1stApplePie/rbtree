#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>
/*
* malloc - void* malloc(size_t size);
  * 함수 호출 시 메모리의 크기를 바이트 단위로 전달하면 그 크기만큼 메모리 할당
  * 할당한 메모리의 주소(첫 바이트의 주소)를 리턴
  * 메모리 할당에 실패하면 NULL 리턴
  * ex) int* i = (int*) malloc (sizeof(int));
   
* free - void free(void* ptr);
  * 할당한 메모리를 해제하는 함수
  * ex
  * arr_2 = (int*) malloc(sizeof(int)*5);
  * free(arr_2);

* calloc - void* calloc(size_t ele_count, size_t ele_size)
  * malloc함수와 같은 기능을 지님

* malloc, calloc의 차이점
  * malloc은 할당된 공간의 값은 바꾸지 않는다.
  * calloc은 할당된 공간의 값을 모두 0으로 바꾼다.
*/

rbtree *new_rbtree(void) {
  // Dynamic alloc for tree
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  // Dynamic alloc for node
  node_t *NIL = (node_t *)calloc(1, sizeof(node_t));
  NIL->color = RBTREE_BLACK;

  p->nil = NIL;
  p->root = NIL;
  return p;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // create new node
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node->left = t->nil;
  new_node->right = t->nil;
  new_node->parent = t->nil;

  // insert new node
  // if root is null, insert root node
  if (t->root == t->nil) {
    new_node->color = RBTREE_BLACK; // root node color: black
    t->root = new_node;
  }
  else {
    bst_insert(t, new_node);
    rbtree_insert_fixup(t, new_node);
  }

  return t->root;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current_node = t->root;

  while (current_node->key != key) {

    if (current_node->right == t->nil && current_node->left == t->nil) {
      return NULL;
    }

    if (key < current_node->key) {
      current_node = current_node->left;
    }
    else {
      current_node = current_node->right;
    }
  }

  return current_node;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *current_node = t->root;

  while (current_node->left != t->nil) {
    current_node = current_node->left;
  }
  return current_node;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *current_node = t->root;

  while (current_node->right != t->nil) {
    current_node = current_node->right;
  }
  return current_node;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

void *bst_insert(rbtree *t, node_t *new_node) {
  // insert new node
  node_t *parent_node = t->root;

  while(1) {
    if (new_node->key < parent_node->key) {
      if (parent_node->left == t->nil) {
        parent_node->left = new_node;
        new_node->parent = parent_node;
        break;
      }
      else parent_node = parent_node->left;
    }

    else {
      if (parent_node->right == t->nil){
        parent_node->right = new_node;
        new_node->parent = parent_node;
        break;
      }
      else parent_node = parent_node->right;
    }
  }
}

void *rbtree_insert_fixup(rbtree *t, node_t *current_node) {
  // A red node does not have a red child

  while (current_node->parent->color == RBTREE_RED){
    if (current_node->parent == current_node->parent->parent->left){  // case 1, 2, 3
      node_t *uncle_node = current_node->parent->parent->right;

      // case 1
      if (uncle_node->color == RBTREE_RED) {
        current_node->parent->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        current_node->parent->parent->color = RBTREE_RED;
        current_node = current_node->parent->parent;
      }

      // case 2
      else {
        if (current_node == current_node->parent->right){
          current_node = current_node->parent;
          rbtree_rotate(t, current_node, ROTATE_LEFT);
        } 
        // case 3
        current_node->parent->color = RBTREE_BLACK;
        current_node->parent->parent->color = RBTREE_RED;
        rbtree_rotate(t, current_node->parent->parent, ROTATE_RIGHT);
      }
    }

    else if (current_node->parent == current_node->parent->parent->right) {
      node_t *uncle_node = current_node->parent->parent->left;

      // case 1
      if (uncle_node->color == RBTREE_RED) {
        current_node->parent->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        current_node->parent->parent->color = RBTREE_RED;
        current_node = current_node->parent->parent;
      }

      // case 2
      else {
        if (current_node == current_node->parent->left){
          current_node = current_node->parent;
          rbtree_rotate(t, current_node, ROTATE_RIGHT);
        }

        // case 3
        current_node->parent->color = RBTREE_BLACK;
        current_node->parent->parent->color = RBTREE_RED;
        rbtree_rotate(t, current_node->parent->parent, ROTATE_LEFT);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

void *rbtree_rotate(rbtree *t, node_t *current_node, const rotate_dir_t rotate_dir) {
  if (rotate_dir == ROTATE_LEFT) {
    // set right node
    node_t *right_node = current_node->right; 
    
    // move right node's left subtree to current node's right subtree      
    current_node->right = right_node->left;                 
    if (right_node->left != t->nil) {
      right_node->left->parent = current_node;              
    }

    // set right node's parent -> current node's parent
    right_node->parent = current_node->parent;
    if (current_node->parent == t->nil){
      t->root = right_node;
    }

    else if (current_node == current_node->parent->left){
      current_node->parent->left = right_node;
    }
    else {
      current_node->parent->right = right_node;
    }
    right_node->left = current_node;
    current_node->parent = right_node;
  }

  else if (rotate_dir == ROTATE_RIGHT){
    node_t *left_node = current_node->left;
    current_node->left = left_node->right;
    if (left_node->right != t->nil) {
      left_node->right->parent = current_node;
    }
    left_node->parent = current_node->parent;
    if (current_node->parent == t->nil){
      t->root = left_node;
    }
    else if (current_node == current_node->parent->right){
      current_node->parent->right = left_node;
    }
    else {
      current_node->parent->left = left_node;
    }
    left_node->right = current_node;
    current_node->parent = left_node;
  }
}

void rbtree_inorder(node_t *root) {
    if (root != NULL) {
        rbtree_inorder(root->left);
        printf("Key: %d, Color: %d\n", root->key, root->color);
        rbtree_inorder(root->right);
    }
}