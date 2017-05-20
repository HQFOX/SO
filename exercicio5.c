#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Processo{
	int id;
	int arrival;
	int tempCPU;
} Processo;

Processo new_process(int id,int arrival, int tempCPU)
{
	Processo *processo = malloc(sizeof(Processo));
	processo -> id = id;
	processo -> arrival = arrival;
	processo -> tempCPU = tempCPU;
	return *processo;
}
void escalonamento()
{
	
}

void read_process(char* linha,int i)
{
	printf("linha lida: %s", linha);
	int j = 0;
	char idString[10];
	char tempCPU[10];
	
	while(linha[j] != ' ')
	{
		idString[j] = linha [j];
	}
	
	j++;
	int l = 0;
	
	while(linha[j] != '\0')
	{
		tempCPU[l] = linha[j];
	}
	
	
}

void open_file(char *buffer)
{
	FILE* ficheiro;
	char* linha = NULL;
	size_t len = 0;
	ssize_t read;
	int i = 0;
	
	ficheiro = fopen("input.txt","r");
	if ( ficheiro == NULL ) 
	{
		puts("erro");
	}
	else
	{
		while((read = getline(&linha, &len, ficheiro)) != -1)
		{
			i++;
			//~ printf("linha lida: %s",linha);
			read_process(linha,i);
		}
	}
	
	fclose(ficheiro);
	
}
void print_file(char *buffer)
{
	//~ int i = 0;
	printf("\n");
	printf("%s\n",buffer);
}


int main(void)
{
	char buffer[100];
	
	open_file(buffer);
	
	return 0;
}
