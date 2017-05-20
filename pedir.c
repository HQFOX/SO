#include <stdio.h>


void le_ficheiro(char* nome)
{
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
		read = getline (&programa, &len, ficheiro);
		while(programa[tamanho] != '\0')
		{
			tamanho += 1;
		}
	}

	printf("programa: %s", programa);
	printf("tamanho: %d \n\n", tamanho);
}

int main (void)
{
	
	while (1)
	{

		char nome_programa[100];
		printf("Nome Programa: ");

		scanf("%s", nome_programa);
		int i = 0;

		while(nome_programa[i] != '\0')
		{
			i++;
		}

		if (nome_programa[i-3] == '.' && nome_programa[i-2] == 'i' && nome_programa[i-1] == 'n')
		{
			printf("Nome Inserido: %s \n \n", nome_programa);
			le_ficheiro(nome_programa);

		}
		else{
			printf("%s\n \n", "Extensão inválida");
		}

		// fazer cenas com o nome do programa


	}
	return 0;
}
