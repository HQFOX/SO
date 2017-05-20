#include <pthread.h>
#include <stdio.h>

void* print_xs (void* unused)
{
	int i = 0;
	while (i<10) 
	{
		fputc( 'O', stderr);
		i++;
	}
	return NULL;
}
void* print_ys (void* unused)
{
	int i = 0;
	while (i<10) 
	{
		fputc( 'x', stderr);
		i++;
	}
	return NULL;
}

int main ()
{
	pthread_t thread_id_x, thread_id_y;
	
	pthread_create (&thread_id_x, NULL, &print_xs, NULL);
	pthread_create (&thread_id_y, NULL, &print_ys, NULL);
	pthread_join(thread_id_x,NULL);
	pthread_join(thread_id_y,NULL);
	return 0;
}


