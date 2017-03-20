// the queue implemented by a linked list with a head
struct queue_node {
	void              *ele;
	struct queue_node *next;
};

typedef struct queue_node  qNode;
typedef struct queue_node* qPtr;

struct queue_head {
	qPtr first;
	qPtr last;
};

typedef struct queue_head* qHead;


//initializate a queue
void init_queue();

//insert a ele to a queue
void insert_queue(qHead queue,qPtr node);

//get the first queue and remove it from queue
void *pop_queue(qHead queue);

//destroy the queue
void delete_queue(qHead queue);

