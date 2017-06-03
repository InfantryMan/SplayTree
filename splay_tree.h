#ifndef SPLAY_H
#define SPLAY_H

typedef int (*comparator)(void *left, void *right);

typedef struct splay_node {
  struct splay_node *parent, *left, *right;
  void *value;
} splay_node;

typedef struct splay_tree {
  splay_node *root;
  comparator comp;
  int size;
} splay_tree;

void store(splay_node *node, splay_tree ***out);
void mark_gp(splay_node *child);
splay_tree* new_tree(comparator comp);
splay_node* insert(splay_tree *tree, void *value);
splay_node* find(splay_tree *tree, void *value);
splay_node* first(splay_tree *tree);
splay_node* next(splay_node *node);
splay_node* last(splay_tree *tree);
splay_tree* contents(splay_tree *tree);
void delete(splay_tree *tree, void *value);
void delete_hint(splay_tree *tree, splay_node *node);
splay_node* find(splay_tree *tree, void *value);


#endif
