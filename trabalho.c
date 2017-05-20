#include <stdio.h>
#include <stdlib.h>
#include <string.h>	//para output
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include "queue.c"



// Variaveis Globais Configuraveis

int max_process_number = 6;
int quantum = 3;
int max_ready = 4;

int tempo;
int program_number;
int run_counter;
int blocked_counter;

Queue *NEW;
Queue *READY;
Queue *RUN;
Queue *BLOCKED;
Queue *EXIT;

pthread_t thread_id;


void running(Node *n);
Programa *read_program(char *s,Programa *p);
void enter_new(Programa* p);
void * ask_input (void * arg);
void release(Node* n);
void output();
void escreve_ficheiro(int* scheduler);
void inst_fork();
/*	FUNCOES DO PROGRAMA */

void le_ficheiro(char* nome)	//retorna 1 se tudo correr bem, 0 caso contrario 
{															//mete o processo na queue NEW
	FILE* ficheiro;
	char* programa = NULL;
	size_t len = 0;
	ssize_t read;
	int tamanho = 0;

	ficheiro = fopen (nome, "r");

	if (ficheiro == NULL) {
		printf ("Ficheiro não encontrado!\n \n");
	} 
	else {
		program_number++;
		read = getline (&programa, &len, ficheiro);
		while(programa[tamanho] != '\0')
		{
			tamanho += 1;
			
		}
		Programa *p = new_programa(program_number);
		print_programa(read_program(programa,p));
		enter_new(p);
	}

	printf("programa: %s", programa);
	printf("tamanho: %d \n\n", tamanho);
	pthread_create (&thread_id, NULL, &ask_input, NULL);
}
void * ask_input (void * arg)
{

		char input[100];
		printf("\nNome Programa: ");

		scanf("%s", input);
		int i = 0;

		while(input[i] != '\0')
		{
			i++;
		}

		if (input[i-3] == '.' && input[i-2] == 'i' && input[i-1] == 'n')
		{
			le_ficheiro(input);
			
		}
		else{
			printf("%s\n \n", "Extensão inválida");
			ask_input(NULL);
		}
	return NULL;
}

Programa *read_program(char *s,Programa *p)	//mete as instrucoes no programa sem espaços 
{

	int i = 0;
	int j = 0;
	
	while(s[i] != '\0')
	{
		if(s[i] != ' ')
		{	
			p -> instructions[j] = s[i];
			j++;
		}
		i++;
	}
	p -> size = j;
	return p;
}
void enter_new(Programa* p)
{
	if(NEW -> sizeQueue < NEW -> limit)
	{
		//~ strcpy(p -> estado,"NEW");
		Node *n = new_node(p);
		enqueue(NEW,n);
	}
}

/*	------------------ */



void enter_ready(Node* n)
{
	if(READY -> sizeQueue < READY -> limit)
	{
		enqueue(READY,n);
	}
}

void enter_blocked(Node* n)
{
	if(BLOCKED -> sizeQueue < BLOCKED -> limit)
	{
		puts("blocked");
		enqueue(BLOCKED,n);
	}
}
void admit()
{
	if(NEW -> head != NULL)
	{
		if(READY -> sizeQueue < max_ready )
		{
			enter_ready(dequeue(NEW));
		}
	}
}

void dispatch()		//manda processos para o estado RUN , poem o run_counter a zero
{
	if(RUN -> head == NULL && READY -> head != NULL)
	{
		Node * n = dequeue(READY);
		run_counter = 0;
		enqueue(RUN,n);
	}
}

void timeout()
{
	enter_ready(dequeue(RUN));
}

void event_wait()
{
	enter_blocked(dequeue(RUN));
}

void event_occurs()
{
	enter_ready(dequeue(BLOCKED));
}


void release(Node* n)
{
	enqueue(EXIT,n);
}

void run()	//funcao do estado RUN
{
	if(RUN -> head != NULL)
	{
		if(RUN -> head -> element -> instructions[RUN -> head -> element -> PC] == '1')
		{
			RUN -> head -> element -> PC++;
		}
		else if (RUN -> head -> element -> instructions[RUN -> head -> element -> PC] == '2')
		{
			RUN -> head -> element -> PC++;
			event_wait();
			
		}
		else if (RUN -> head -> element -> instructions[RUN -> head -> element -> PC] == '3')
		{
			if(RUN -> head -> element -> jump_counter < 10)
			{
				RUN -> head -> element -> PC = 0;
				RUN -> head -> element -> jump_counter++;
			}
			else
			{
				RUN -> head -> element -> instructions[RUN -> head -> element -> PC] = '1';
				RUN -> head -> element -> PC++;
			}
		}
		else if (RUN -> head -> element -> instructions[RUN -> head -> element -> PC] == '4')
		{
			inst_fork();
			 //o pc é incrementado no int_fork
		}
		else if (RUN -> head -> element -> instructions[RUN -> head -> element -> PC] == '0')
		{
			release(dequeue(RUN));
			run_counter = 0;
		}
		run_counter++;
		if(run_counter == quantum)
		{
			timeout();
		}
	}
}


void blocked()	//funcao do estado do blocked
{
	if(BLOCKED -> head != NULL)
	{
		if(blocked_counter < 3)
		{
			blocked_counter++;
		}
		else
		{
			event_occurs();
			blocked_counter = 0;
		}
	}
}

void output(){
	int outputS[program_number-1]; //Array de int lol de tamanho fixo com o estado dos programas
	
	if(NEW -> head != NULL)
	{
		Node *current = NEW -> head;
		while(current != NULL)
		{
			//~ strcpy(outputS[((current -> element -> id)-1)],current -> element -> estado);
			outputS[((current -> element -> id)-1)] = 0;
			current = current -> next;
		}
	}
	if(READY -> head != NULL)
	{
		Node *current = READY -> head;
		while(current != NULL)
		{
			//~ strcpy(outputS[((current -> element -> id)-1)],current -> element -> estado);
			outputS[((current -> element -> id)-1)] = 1;
			current = current -> next;
		}
	}
	if(RUN -> head != NULL)
	{
		Node *current = RUN -> head;
		while(current != NULL)
		{
			outputS[((current -> element -> id)-1)] = 2;
			current = current -> next;
		}
	}
	if(BLOCKED -> head != NULL)
	{
		Node *current = BLOCKED -> head;
		while(current != NULL)
		{
			outputS[((current -> element -> id)-1)] = 3;
			current = current -> next;
		}
	}
	if(EXIT -> head != NULL)
	{
		Node *current = EXIT -> head;
		while(current != NULL)
		{
			outputS[((current -> element -> id)-1)] = 4;
			current = current -> next;
		}
	}
	//~ puts("cona");
	//~ printf("\n%d | ",program_number);
	//~ printf("tempo %d | ",tempo);
	//~ if(program_number != 0)
	//~ {
		//~ for(int i = 0; i < program_number ; i++)
		//~ {
			//~ printf("%d | ", outputS[i]);
		//~ }
	//~ }
	//~ printf("\n");
	escreve_ficheiro(outputS);
}

void escreve_ficheiro(int* scheduler){

	FILE *ficheiro;
	ficheiro = fopen ("scheduler.out", "a");

	fprintf(ficheiro, "\n%d |", tempo);
	for (int i = 0; i < program_number; ++i)
	{
		if (scheduler[i] == 0){
			fprintf(ficheiro, " NEW |");
		}
		if (scheduler[i] == 1){
			fprintf(ficheiro, " READY |");
		}
		if (scheduler[i] == 2){
			fprintf(ficheiro, " RUN |");
		}
		if (scheduler[i] == 3){
			fprintf(ficheiro, " BLOCKED |");
		}
		if (scheduler[i] == 4){
			fprintf(ficheiro, " EXIT |");
		}
	}

	fclose(ficheiro);
}

void inst_fork()
{
	RUN -> head -> element -> PC++;
	Node *n;
	program_number++;
	Programa *p = new_programa(program_number);
	strcpy(p -> instructions,RUN -> head -> element -> instructions);
	//~ p -> instructions = RUN -> head -> element -> instructions;		//TESTAR , PODE TER BUG COM O GOTOBEGIN
	p -> PC = 	RUN -> head -> element -> PC;
	n = new_node(p); 
	n -> next = NULL;
	enter_ready(n);
}

int main(void)
{
	// Pedido da Settings Configuraveis

	int input_prov;

	puts ("\n -- Settings -- \n");

	puts ("Quantum (0 - default): ");
	scanf("%d", &input_prov);
	if (input_prov != 0)
	{
		quantum = input_prov;
	}

	puts ("Nº Maximo de Processos (0 - default): ");
	scanf("%d", &input_prov);
	if (input_prov != 0)
	{
		max_process_number = input_prov;
	}

	puts ("Nº Maximo de Processos no Ready (0 - default): ");
	scanf("%d", &input_prov);
	if (input_prov != 0)
	{
		max_ready = input_prov;
	}

	tempo = 0;
	program_number = 0;
		
	NEW = new_queue(max_process_number);
	
	READY = new_queue(max_process_number);
	
	BLOCKED = new_queue(max_process_number);
	
	RUN = new_queue(1);
	
	EXIT = new_queue(max_process_number);
	
	pthread_create (&thread_id, NULL, &ask_input, NULL);
	
	while (program_number < max_process_number)
	{
		admit();	//adiciona se o tamanho de READY for < 4 
		dispatch();	//adiciona ao estado RUN se este tiver vazio  
		run();
		dispatch();
		blocked();
		dispatch();
		output();
		sleep(0.5);
		tempo++;

	}
	
	//~ pthread_kill
	//~ output();
	return 0;
}

//testar o gotobegin
