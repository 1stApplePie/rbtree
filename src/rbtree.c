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

typedef enum { ROTATE_RIGHT, ROTATE_LEFT } rotate_dir_t;

void *bst_insert(rbtree *, node_t *);
void *rbtree_insert_fixup(rbtree *, node_t *);
void *rbtree_rotate(rbtree *, node_t *, const rotate_dir_t);
void *rbtree_erase_fixup(rbtree *, node_t *);
int rbtree_inorder(node_t *, key_t *, int);
void *rbtree_transplant(rbtree *, node_t *, node_t *);

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

  while (current_node != t->nil) {
    if (current_node->key == key)
      return current_node;
    else if (current_node->key < key)
      current_node = current_node->right;
    else
      current_node = current_node->left;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *current_node = t->root;

  while (current_node->left != t->nil) {
    current_node = current_node->left;
  }
  return current_node;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *current_node = t->root;

  while (current_node->right != t->nil) {
    current_node = current_node->right;
  }
  return current_node;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *delete_node = p;
  node_t *new_node;
  color_t delete_node_original_color = delete_node->color;

  if (p->left == t->nil){
    new_node = p->right;
    rbtree_transplant(t, p, p->right);
  }

  else if (p->right == t->nil){
    new_node = p->left;
    rbtree_transplant(t, p, p->left);
  }

  else {
    node_t *temp_node = p->right;
    while(1){
      if (temp_node->left == t->nil)
        break;
      temp_node = temp_node->left;
    }
    delete_node = temp_node;

    delete_node_original_color = delete_node->color;
    new_node = delete_node->right;
    if (delete_node->parent == p){
      new_node->parent = delete_node;
    }
    else{
      rbtree_transplant(t, delete_node, delete_node->right);
      delete_node->right = p->right;
      delete_node->right->parent = delete_node;
    }
    rbtree_transplant(t, p, delete_node);
    delete_node->left = p->left;
    delete_node->left->parent = delete_node;
    delete_node->color = p->color;
  }
  if (delete_node_original_color == RBTREE_BLACK){
    rbtree_erase_fixup(t, new_node);
  }
  free(p);
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  rbtree_inorder(t->root, arr, 0);
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

void *rbtree_erase_fixup(rbtree *t, node_t *p){
  while (p != t->root && p->color == RBTREE_BLACK){
    if (p == p->parent->left){
      node_t *brother_node = p->parent->right;
      if (brother_node->color == RBTREE_RED){
        brother_node->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        rbtree_rotate(t, p->parent, ROTATE_LEFT);
        brother_node = p->parent->right;
      }

      if (brother_node->left->color == RBTREE_BLACK && brother_node->right->color == RBTREE_BLACK){
        brother_node->color = RBTREE_RED;
        p = p->parent;
      }

      else if (brother_node->right->color == RBTREE_BLACK){
        brother_node->left->color = RBTREE_BLACK;
        brother_node->color = RBTREE_RED;
        rbtree_rotate(t, brother_node, ROTATE_RIGHT);
        brother_node = p->parent->right;
      }

      else{
        if (brother_node->right->color == RBTREE_BLACK){
          brother_node->left->color = RBTREE_BLACK;
          brother_node->color = RBTREE_RED;
          rbtree_rotate(t, brother_node, ROTATE_RIGHT);
          brother_node = p->parent->right;
        }
        brother_node->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        brother_node->right->color = RBTREE_BLACK;
        rbtree_rotate(t, p->parent, ROTATE_LEFT);
        p = t->root;
      }
    }

    else {
      node_t *brother_node = p->parent->left;
      if (brother_node->color == RBTREE_RED){
        brother_node ->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        rbtree_rotate(t, p->parent, ROTATE_RIGHT);
        brother_node = p->parent->left;
      }

      if (brother_node->right->color == RBTREE_BLACK && brother_node->left->color == RBTREE_BLACK){
        brother_node->color = RBTREE_RED;
        p = p->parent;
      }

      else if (brother_node->left->color == RBTREE_BLACK){
        brother_node->right->color = RBTREE_BLACK;
        brother_node->color = RBTREE_RED;
        rbtree_rotate(t, brother_node, ROTATE_LEFT);
        brother_node = p->parent->left;
      }

      else{
        if (brother_node->left->color == RBTREE_BLACK){
          brother_node->right->color = RBTREE_BLACK;
          brother_node->color = RBTREE_RED;
          rbtree_rotate(t, brother_node, ROTATE_LEFT);
          brother_node = p->parent->left;
        }
        brother_node->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        brother_node->left->color = RBTREE_BLACK;
        rbtree_rotate(t, p->parent, ROTATE_RIGHT);
        p = t->root;
      }
    }
  }
  p->color = RBTREE_BLACK;
}

void *rbtree_transplant(rbtree *t, node_t *delete_node, node_t *new_node){
  if (delete_node->parent == t->nil){
    t->root = new_node;
  }
  else if (delete_node == delete_node->parent->left){
    delete_node->parent->left = new_node;
  }
  else {
    delete_node->parent->right = new_node;
  }

  new_node->parent = delete_node->parent;
}

int rbtree_inorder(node_t *root, key_t *arr, int i) {
    if (root != NULL) {
        i = rbtree_inorder(root->left, arr, i);
        if(root->right != NULL && root->left != NULL){
          arr[i] = root->key;
          i++;
        }
        i = rbtree_inorder(root->right, arr, i);
    }
    return i;
}