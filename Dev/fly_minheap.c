/********************************
A min-heap implemented based on a binary tree.

Author: Andrew lin
********************************/
#include "fly_minheap.h"

fly_tree_head fly_init_minheap()
{
	fly_tree_head tHead = malloc(struct fly_tree_head);
	if (tHead == NULL) {
		printf("malloc error.\n");
		return NULL;
	}
	memset(tHead, '/0', sizeof(struct fly_tree_head));

	fly_tree_node tNode = malloc(struct fly_tree_node);
	if (tNode == NULL) {
		printf("malloc error.\n");
		return NULL;
	}
	tNode->left = tNode->right = tNode->ele = NULL;

    tHead->minheap_top = tNode;

    return tHead;
}

int fly_insert_minheap(fly_tree_head pNode, struct timeval *tv)
{
	if (pNode == NULL || ele == NULL) {
		printf("pNode or ele is NULL.\n");
		return -1;
	}

    
}
