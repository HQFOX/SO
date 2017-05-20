#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

typedef struct Programa
{
	int id;
	int PC;
	char* instructions;
	int size;	//temporario para fazer o print
	int jump_counter;
	
}Programa;


/*	FUNCOES DO NODE DE QUEUE */ 

typedef struct Node		//node da fila
{
	Programa *element;
	Node *next;
}Node;

Node * new_node(Programa * e)
{
	Node *n = malloc(sizeof(Node));
	n -> element = e;
	n -> next = NULL;
	
	return n;
}
/*	------------------ */

/*	FUNCOES DA QUEUE */ 

typedef struct Queue	//fila de espera
{	
	int sizeQueue;
	int limit;
	Node *head;
}Queue;
	
Queue * new_queue(int limit)
{
	Queue *q = malloc(sizeof(Queue));
	q -> limit = limit;
	q -> head = NULL;
	q -> sizeQueue = 0;
	
	return q;
}

void enqueue(Queue *q,Node *n)
{
	if(q -> sizeQueue < q -> limit)
	{
		if( q -> head == NULL ) 
		{
			q -> head = n;
		}
		else
		{
			Node * current = q -> head;
			while( current -> next != NULL)
			{
				current = current -> next;
			}
			current -> next = n;
		}
		q -> sizeQueue++;
	}
}
Node * dequeue(Queue * q)
{
	if(q -> sizeQueue != 0)
	{
		Node *n = q -> head;
		q -> head = q-> head -> next;
		q -> sizeQueue--;
		n -> next = NULL;
		
		return n;
	}
	return NULL;
}

void print_queue(Queue * q)
{
	if(q -> head != NULL)
	{
		Node * current = q -> head;
		
		while(current != NULL)
		{
			printf(" id: %d " ,current -> element -> id);
			current = current -> next;
		}
		free(current);
	}
}

void queue_destroy(Queue *q)
{
	if(q -> head != NULL)
	{
		Node * prev;
		Node * current = q -> head;
		
		while(current != NULL)
		{
			prev = current;
			free(prev);
			current = current -> next;
		}
		free(current);
		free(prev);
	}
	free(q -> head);
	free(q);
}
/*	------------------ */

Programa * new_programa(int id)
{

	Programa *p = malloc(sizeof(p));
	p -> instructions = malloc(100 *sizeof(char));
	p -> id = id;
	p -> PC = 0;
	p -> jump_counter = 0;
	return p;
}



void print_programa(Programa *p)
{
	printf("\nid : %d\nInstrucoes:",p -> id);
	int i = 0;
	while( i< p -> size )
	{
		printf("%c ", p -> instructions[i]);
		i++;
	}
}
