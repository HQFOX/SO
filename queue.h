/* queue interface */

#include <stdbool.h>

typedef struct Programa Programa;
typedef struct Node Node;
typedef struct Queue Queue;


Programa * new_programa(int id);
Node * new_node(Programa * e);
Queue * new_queue(int limit);

void print_programa(Programa *p);

void enqueue(Queue *q,Node *n);
Node * dequeue(Queue * q);
void print_queue(Queue * q);

