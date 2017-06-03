#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "splay_tree.h"

extern int err;

void rotate(splay_node *child);

splay_node* leftmost(splay_node *node);
splay_node* rightmost(splay_node *node);

void zig(splay_node *x, splay_node *p);
void zigzig(splay_node *x, splay_node *p);
void zigzag(splay_node *x, splay_node *p);
void splay(splay_tree *tree, splay_node *x) {
  while(1) {
    splay_node *p = x->parent;
    if(p==NULL){
      tree->root=x;
      return;
    }
    splay_node *g = p->parent;
    if (p->parent == NULL)
      zig(x,p);
    else
      if((x == p->left && p == g->left) || (x == p->right && p==g->right))
        zigzig(x,p);
      else
        zigzag(x,p);
  }
}

void zig(splay_node *x, splay_node *p) {
  rotate(x);
}

void zigzig(splay_node *x, splay_node *p) {
  rotate(p);
  rotate(x);
}

void zigzag(splay_node *x, splay_node *p) {
  rotate(x);
  rotate(x);
}

void rotate(splay_node *child) {
  splay_node *parent = child->parent;
  assert(parent !=NULL);
  if (parent->left ==child) {
    mark_gp(child);
    parent->left = child->right;
    if(child->right != NULL)
      child->right = parent;
    child->right = parent;
  } else {
    mark_gp(child);
    parent->right = child->left;
    if(child->left != NULL)
      child->left->parent=parent;
    child->left=parent;
  }
}

splay_tree* new_tree(comparator comp) {
    splay_tree *new = malloc(sizeof(splay_tree));
    if (new ==NULL) {
        fprintf(stderr, "No available memory");
        err = 1;
        return;
    }
    new->comp = comp;
    new->root = NULL;
    new->size = 0;
    return new;
}


void store(splay_node *node, splay_tree ***out) {
    if(node->left != NULL)
        store(node->left, out);
    **out = node->value;
    (*out)++;
    if(node->right != NULL)
        store(node->right, out);
}

void mark_gp(splay_node *child) {
	splay_node *parent = child->parent;
	splay_node *grand = parent->parent;
	child->parent = grand;
	if(grand == NULL)
		return;
	if (grand->left == parent)
		grand->left = child;
	else
		grand->right = child;
}

splay_node* insert(splay_tree *tree, void *value) {
    splay_node *new = malloc(sizeof(splay_node));
    if (new == NULL) {
        fprintf(stderr, "No available memory");
        err = 1;
        return;
    }
    new->value = value;
    new->left = NULL;
    new->right = NULL;
    if(tree->root == NULL) {
        tree->root = new;
        new->parent = NULL;
    } else {
        splay_node *curr = tree->root;
        splay_node *parent;
        int left;
        while(curr != NULL) {
            parent = curr;
            if(tree->comp(new->value, curr->value) < 0) {
                left = 1;
                curr = curr->left;
            } else {
                left = 0;
                curr = curr->right;
            }
        }
        new->parent = parent;
        if(left)
            parent->left = new;
        else
            parent->right = new;
    }
    splay(tree, new);
    tree->size++;
    return new;
}

splay_node* first(splay_tree *tree) {
    return leftmost(tree->root);
}


splay_node* next(splay_node *node) {
    if(node->right != NULL)
        return leftmost(node->right);
    while(node->parent != NULL && node == node->parent->right)
        node = node->parent;
    return node->parent;
}

splay_node* last(splay_tree *tree) {
    return rightmost(tree->root);
}

splay_node* leftmost(splay_node *node) {
    splay_node *parent = NULL;
    while(node != NULL) {
        parent = node;
        node = node->left;
    }
    return parent;
}

splay_node* rightmost(splay_node *node) {
    splay_node *parent = NULL;
    while(node != NULL) {
        parent = node;
        node = node->right;
    }
    return parent;
}

splay_tree* contents(splay_tree *tree) {
    if(tree->size == 0)
        return NULL;
    splay_tree **out = malloc(tree->size * sizeof(void*));
    if (out == NULL) {
        fprintf(stderr, "No available memory");
        err = 1;
        return;
    }
    splay_tree ***tmp = &out;
    store(tree->root, tmp);
    return out - tree->size;
}

splay_node* find(splay_tree *tree, void *value) {
    splay_node *curr = tree->root;
    int found = 0;
    while(curr != NULL && !found) {
        int relation = tree->comp(value, curr->value);
        if(relation == 0) {
            found = 1;
        } else if(relation < 0) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    if(curr != NULL)
        splay(tree, curr);
    return curr;
}

void delete(splay_tree *tree, void *value) {
    splay_node *node = find(tree, value);
    delete_hint(tree, node);
}

void delete_hint(splay_tree *tree, splay_node *node) {
    if(node == NULL)
        return;
    splay(tree, node); 
    if(node->left == NULL) {
        tree->root = node->right;
        if(tree->root != NULL)
            tree->root->parent = NULL;
    } else if(node->right == NULL) {
        tree->root = node->left;
        tree->root->parent = NULL;
    } else {
        splay_node *x = leftmost(node->right);
        if(x->parent != node) {
            x->parent->left = x->right;
            if(x->right != NULL)
                x->right->parent = x->parent;
            x->right = node->right;
            x->right->parent = x;
        }
        tree->root = x;
        x->parent = NULL;
        x->left = node->left;
        x->left->parent = x;
    }
    free(node);
    tree->size--;
}