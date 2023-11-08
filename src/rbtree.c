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

void *bstree_insert(rbtree *, node_t *);
void *rbtree_insert_fixup(rbtree *, node_t *);
void *rbtree_rotate(rbtree *, node_t *, const rotate_dir_t);
void *rbtree_erase_fixup(rbtree *, node_t *);
int rbtree_inorder(node_t *, key_t *, int);
void *rbtree_transplant(rbtree *, node_t *, node_t *);

/* 
* @details Create a new red-black tree (rbtree) and initialize its properties.
* @return A pointer to the newly created rbtree.
*/
rbtree *new_rbtree(void) {
  /*
  * void* calloc(size_t element_count, size_t element_size)
    * element size 크기의 변수를 element count개 만큼 저장할 수 있는 메모리 공간을 할당
  */

  // Dynamic alloc for tree
  // size of rbtree struct: 16 -> node_t pointer * 2
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));  // 16바이트 크기의 변수 1개를 담을 수 있는 공간을 동적 할당

  // Dynamic alloc for node
  // node_t: {color_t, key_t, struct node_t * 3}
  // size of node_t: 32 -> color_t: 4, key_t: 4, pointer * 3: 24
  node_t *NIL = (node_t *)calloc(1, sizeof(node_t));  // 32바이트 크기의 변수 1개를 담을 수 있는 공간을 동적 할당

  NIL->color = RBTREE_BLACK;

  p->nil = NIL;
  p->root = NIL;
  return p;
}

/* 
* @details Inserts a new node with the specified key into the red-black tree (rbtree).
* @param[in] t - A pointer to the rbtree.
* @param key - The key to be inserted.
* @return A pointer to the root of the modified rbtree.
 */
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
    bstree_insert(t, new_node);
    rbtree_insert_fixup(t, new_node);
  }

  return t->root;
}

/*
 * @details Deallocates memory for the entire red-black tree (rbtree) structure.
 * @param[in] t - A pointer to the rbtree to be deleted.
 * @return void
 */
void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

/*
* @details Finds a node with the specified key in the red-black tree (rbtree).
* @param[in] t - A pointer to the rbtree to search in.
* @param[in] key - The key value to search for.
* @return node_t - A pointer to the found node, or NULL if not found.
*/
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

/*
* @details Finds the node with the minimum key in the red-black tree (rbtree).
* @param[in] t - A pointer to the rbtree to search in.
* @return node_t - A pointer to the node with the minimum key.
*/
node_t *rbtree_min(const rbtree *t) {
  node_t *current_node = t->root;

  while (current_node->left != t->nil) {
    current_node = current_node->left;
  }
  return current_node;
}

/*
* @details Finds the node with the maximum key in the red-black tree (rbtree).
* @param[in] t - A pointer to the rbtree to search in.
* @return node_t - A pointer to the node with the maximum key.
*/
node_t *rbtree_max(const rbtree *t) {
  node_t *current_node = t->root;

  while (current_node->right != t->nil) {
    current_node = current_node->right;
  }
  return current_node;
}

/*
* @details Deletes a node with a given key from the red-black tree (rbtree).
* @param[in] t - A pointer to the rbtree.
* @param[in] p - A pointer to the node to delete.
* @return int - Returns 0 on successful deletion.
*/
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

/*
 * @details Performs an inorder traversal of the rbtree and stores keys in an array.
 * @param[in] t - A pointer to the rbtree.
 * @param[out] arr - A pointer to the array to store the keys.
 * @param[in] n - The maximum number of keys to store in the array.
 * @return int - Returns 0 on successful traversal.
 */
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  rbtree_inorder(t->root, arr, 0);
  return 0;
}

/* 
* @details  Binary search tree insert function
* @param[in]  rbtree_struct_pointer Red-Black Tree being inserted into.
* @param[in]  node_t_struct_pointer Node to be inserted.
* @return  void
*/
void *bstree_insert(rbtree *t, node_t *new_node) {
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

/* 
* @details  Fixup Red Black Tree after binary search tree insert
* @param[in]  rbtree_struct_pointer Red-Black Tree to Fixup.
* @param[in]  node_t_struct_pointer Pivot node pointer.
* @return  void
*/
void *rbtree_insert_fixup(rbtree *t, node_t *current_node) {
  // if parent node's color is red
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

/* 
* @details  rotate 2 nodes - current node, child node, rotate direction - rotate_dir_t
* @param[in]  rbtree_struct_pointer Red-Black Tree to Rotate.
* @param[in]  node_t_struct_pointer Pivot node pointer.
* @param[in]  rotate_direction left or right direction, rotate_dir_t = {ROTATE_LEFT, ROTATE_RIGHT}.
* @return  void
*/
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

    // reconnect current node <-> right node
    right_node->left = current_node;
    current_node->parent = right_node;
  }

  else if (rotate_dir == ROTATE_RIGHT){
    // set left node
    node_t *left_node = current_node->left;

    // move left node's right subtree to current node's left subtree
    current_node->left = left_node->right;
    if (left_node->right != t->nil) {
      left_node->right->parent = current_node;
    }

    // set left node's parent -> current node's parent
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

    // reconnect current node <-> right node
    left_node->right = current_node;
    current_node->parent = left_node;
  }
}

/*
* @details Fixes the rbtree properties after erasing a node, preserving the rbtree structure.
* @param[in] t - A pointer to the rbtree.
* @param[in] p - A pointer to the node to fix the rbtree properties from.
* @return void
*/
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

/*
* @details Replaces one subtree in the rbtree with another subtree.
* @param[in] t - A pointer to the rbtree.
* @param[in] delete_node - A pointer to the node to be replaced.
* @param[in] new_node - A pointer to the new subtree.
* @return void
*/
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

/*
* @details Performs an inorder traversal of the rbtree and stores the keys in an array.
* @param[in] root - A pointer to the root node of the subtree to traverse.
* @param[out] arr - A pointer to the array for storing the keys.
* @param[in] i - The current index in the array.
* @return The updated index in the array after storing the keys.
*/
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