/********************************
A min-heap implemented based on a binary tree.

Author: Andrew lin
********************************/
#ifndef _FLY_MINHEAP_H
#define _FLY_MINHEAP_H

struct fly_tree_node {
    struct timeval *tv; //temporarily use this type
    fly_tree_node *left;
    fly_tree_node *right;
};

struct fly_tree_head {
	fly_tree_node minheap_top; //use this pointed to the binary tree's top.
};

typedef struct fly_tree_head* fly_tree_head;
typedef struct fly_tree_node* fly_tree_node;

fly_tree_head fly_init_minheap();

int fly_insert_minheap(fly_tree_head pNode, struct timeval *tv);

fly_tree_node fly_pop_minheap(fly_tree_node pNode);

int fly_remove_minheap(fly_tree_node pNode, struct timeval *tv);

int fly_empty_minheap(fly_tree_node pNode);

int fly_size_minheap(fly_tree_node pNode);

#endif
