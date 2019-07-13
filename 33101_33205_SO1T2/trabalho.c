#include <stdio.h>
#include <stdlib.h>
#include <string.h>	//para output
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include "queue.c"

#define MEMSIZE 50

// Variaveis Globais Configuraveis

int max_process_number = 6;
int quantum = 3;
int max_ready = 4;
int flag = 1;

char MEM[MEMSIZE];
int tempo;
int program_number;
int run_counter;
int blocked_counter;
int occupied;

Queue *NEW;
Queue *READY;
Queue *RUN;
Queue *BLOCKED;
Queue *EXIT;

pthread_t thread_id;


void running(Node *n);
Programa *read_program(char *s,Programa *p);
void enter_new(Programa* p);
int best_fit(int tamanho);
int worst_fit(int tamanho);
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
		if( read_program(programa,p) != NULL)
		{
			enter_new(p);
		}
		else
		{
			program_number--;
		}
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
	
	char buffer[100];
	
	while(s[i] != '\0')
	{
		if(s[i] != ' ' )
			if(s[i] >= '0' && s[i] <= '4')
			{	
				buffer[j] = s[i];
				j++;
			}
			i++;
	}
	p -> size = j;
	if (occupied + p -> size <= MEMSIZE)	//ve se a memoria está cheia
	{
		occupied = occupied + p -> size;
		if(flag == 1)
			i = best_fit(j);
		else
			i = worst_fit(j);
		j = 0;
		p -> instrucoes_mem_pointer = i;
		//~ printf("  pointer %d ", p -> instrucoes_mem_pointer);
		while(j< p -> size)
		{
			MEM[i] = buffer[j];
			i++;
			j++;
		}
		return p;
	}
	else
	{
		printf("\n Memória está cheia – impossível criar processo\n");
	}
	return NULL;
}
void enter_new(Programa* p)
{
	if(NEW -> sizeQueue < NEW -> limit)
	{
			Node *n = new_node(p);
			enqueue(NEW,n);
	}
}

/*	------------------ */

int best_fit(int tamanho) //devolve o indice onde deve ser colocado no MEM mas a "partir do fim" 
{
	int i = 0; ;	//contador para percorrer o array MEM
	int j = 0;		//contador para contar os espaços vazios seguidos
	int menor = 50; //guardar o menor numero de espaços seguidos onde cabe as instruçoes 
	int indice = 0;		//guarda o indice 
	
	while(i<MEMSIZE)
	{
		if(MEM[i] == 'x')
		{
			j++;
		}
		else
		{
			if( j <= menor)
			{	//verifica se o numero de espaços vazios seguidos é menor que o anteriormente guardado
				if (j >= tamanho)
				{	// verifica se este contem espaço para as instruções
					menor = j;
					indice = (i - tamanho);
				}
			}
			j = 0;
		}
		i++;
	}
	if( i - j < menor)
	{
		indice = i - j;
	}
	return indice;
}

int worst_fit(int tamanho)
{
	int i = 0; ;	//contador para percorrer o array MEM
	int j = 0;		//contador para contar os espaços vazios seguidos
	int maior = 0; //guardar o maior numero de espaços seguidos onde cabe as instruçoes 
	int indice = 0;		//guarda o indice 
	
	while(i<MEMSIZE)
	{
		if(MEM[i] == 'x')
		{
			j++;
		}
		else
		{
			if( j >= maior)
			{	//verifica se o numero de espaços vazios seguidos é menor que o anteriormente guardado
				if (j >= tamanho)
				{	// verifica se este contem espaço para as instruções
					maior = j;
					indice = (i - tamanho);
				}
			}
			j = 0;
		}
		i++;
	}
	if( i - j > maior)
	{
		indice = i - j;
	}
	return indice;
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
	
	int i = n -> element -> instrucoes_mem_pointer;
	int j = 0;
	
	while(j < n -> element -> size)
	{
		MEM[i] = 'x';
		j++;
		i++;
	}
	occupied = occupied - n -> element ->size;
}

void run()	//funcao do estado RUN
{
	if(RUN -> head != NULL)
	{
		if(MEM[ (RUN -> head -> element -> instrucoes_mem_pointer + RUN -> head -> element -> PC) ] == '1')
		{
			RUN -> head -> element -> PC++;
		}
		else if(MEM[ (RUN -> head -> element -> instrucoes_mem_pointer + RUN -> head -> element -> PC) ] == '2')
		{
			RUN -> head -> element -> PC++;
			event_wait();
			
		}
		else if(MEM[ (RUN -> head -> element -> instrucoes_mem_pointer + RUN -> head -> element -> PC) ] == '3')
		{
			if(RUN -> head -> element -> jump_counter < 10)
			{
				RUN -> head -> element -> PC = 0;
				RUN -> head -> element -> jump_counter++;
			}
			else
			{
				MEM[ (RUN -> head -> element -> instrucoes_mem_pointer + RUN -> head -> element -> PC) ] = '1';
				RUN -> head -> element -> PC++;
			}
		}
		else if(MEM[ (RUN -> head -> element -> instrucoes_mem_pointer + RUN -> head -> element -> PC) ] == '4')
		{
			inst_fork();
			 //o pc é incrementado no int_fork
		}
		else if(MEM[ (RUN -> head -> element -> instrucoes_mem_pointer + RUN -> head -> element -> PC) ] == '0')
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
	
	
	int outputS[program_number]; //Array de int lol de tamanho fixo com o estado dos programas
	
	if(NEW -> head != NULL)
	{
		Node *current = NEW -> head;
		while(current != NULL)
		{
			outputS[((current -> element -> id)-1)] = 0;
			current = current -> next;
		}
	}
	if(READY -> head != NULL)
	{
		Node *current = READY -> head;
		while(current != NULL)
		{
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
	fprintf(ficheiro,"\n MEM |");
	int i = 0;
	while(i < MEMSIZE)
	{
		fprintf(ficheiro, "%c", MEM[i]);
		i++;
	}

	fclose(ficheiro);
}

void inst_fork()
{
	RUN -> head -> element -> PC++;
	if(occupied + (RUN -> head -> element ->size) <= 50)
	{
		Node *n;
		program_number++;
		occupied = occupied + RUN -> head -> element ->size;
		Programa *p = new_programa(program_number);
		p -> PC = 	RUN -> head -> element -> PC;
		p -> size = RUN -> head -> element -> size;
		
		//passa para o array MEM
		int i = 0;
		if(flag == 1)
			i = best_fit(RUN -> head -> element ->size);
		else
			i = worst_fit(RUN -> head -> element ->size);
		int j = RUN -> head -> element -> instrucoes_mem_pointer;
		p -> instrucoes_mem_pointer = i;
		int l = 0;
		while(l< p -> size)
		{
			MEM[i] = MEM[j];
			i++;
			j++;
			l++;
		}
		// ----------
		
		n = new_node(p); 
		n -> next = NULL;
		enter_ready(n);
	}
	else
	{
		printf("\n Memória está cheia – impossível criar processo\n");
	}
	
}

int main(void)
{
	
	for(int i = 0; i <MEMSIZE ; i++)	//inicializa o array MEM com xxxxx
	{
		MEM[i] = 'x';
	}
	
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
	
	puts ("Best fit -1  Worst fit -2  (0 - default): ");
	scanf("%d", &input_prov);
	if (input_prov != 0)
	{
		flag = input_prov;
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
		//~ output();
		admit();	//adiciona se o tamanho de READY for < 4 
		dispatch();	//adiciona ao estado RUN se este tiver vazio  
		run();
		dispatch();
		blocked();
		dispatch();
		output();
		sleep(1);
		tempo++;

	}
	return 0;
}
