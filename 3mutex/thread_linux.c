#include <stdio.h>
#include <pthread.h>
#include <unistd.h>  // usleep

#define NTHREADS 10
#define avec_mutex

void *thread_function(void *);
#ifdef avec_mutex
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
#endif
int  counter = 0;

int main()
{int d[10];
 pthread_t thread_id[NTHREADS];
 int i, j;

 for (i=0; i < NTHREADS; i++)   
     {d[i]=i;
      pthread_create( &thread_id[i], NULL, thread_function, &d[i] );} // CREATION THREADS
 for(j=0; j < NTHREADS; j++) {pthread_join( thread_id[j], NULL);}     // ATTENTE FIN THREADS
 printf("Final counter value: %d\n", counter);
}

void *thread_function(void *d)
{printf("Thread number %d: %lx\n", *(int *)d, pthread_self());
#ifdef avec_mutex
 pthread_mutex_lock( &mutex1 );
#endif
 usleep(500000); // 500 ms
 counter++;
#ifdef avec_mutex
 pthread_mutex_unlock( &mutex1 );
#endif
}
