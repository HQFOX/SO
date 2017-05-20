#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include 'queue.c'



int cona;

typedef struct
{
   int id;
} ThreadData;

void* thread(void* arg)
{
   ThreadData* data = (ThreadData*) arg;
   
   cona = 100;

   printf("Thread id is %d\n", data->id);

   return NULL;
}

int main(void)
{
   pthread_t tid;
   ThreadData* td = malloc(sizeof(ThreadData));

   td->id = 10;

   pthread_create(&tid, NULL, thread, td);

   pthread_join(tid, NULL);

   printf(" %d ", cona);
   
   free(td);

   return 0;
}
