#include <stdio.h>
#include <pthread.h>

#define BUFFER_SIZE 10
#define BUFFER_USAGE 40

int buffer[BUFFER_SIZE];
int element_buffer = 0;
int next_available_index_prod=0;
int next_available_index_cons = 0;
int usage_count = BUFFER_USAGE;

pthread_cond_t cond_pro = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_con = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *producer(void *arg);
void *consumer(void *arg);

int main(int argc, char const *argv[])
{
	for (int i = 0; i < BUFFER_SIZE; ++i)
	{
		buffer[i] = -1;
	}
	pthread_t thread1,thread2;
	if (pthread_create(&thread1, NULL, &producer, NULL) != 0)
	{
		printf("Producer thread failed to execute.\n");
		exit(1);
	}
	if (pthread_create(&thread2, NULL, &consumer, NULL) != 0)
	{
		printf("Consumer thread failed to execute.\n");
		exit(1);
	}
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	return 0;
}

void *producer(void *arg){
	int i;
	for (;;)
	{
		pthread_mutex_lock(&mutex);
		if (element_buffer == BUFFER_SIZE)
		{
			pthread_cond_wait(&cond_pro, &mutex);
		}
		i = next_available_index_prod;
		if (buffer[i] == -1)
		{
			buffer[next_available_index_prod] = next_available_index_prod;
			element_buffer++;
			printf("Producer added %d element in the buffer\n", i);
		}
		next_available_index_prod = (next_available_index_prod + 1)%BUFFER_SIZE;
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cond_con);
	}
}

void *consumer(void *arg){
	int i;
	for (;;)
	{
		pthread_mutex_lock(&mutex);
		if (element_buffer == 0)
		{
			pthread_cond_wait(&cond_con,&mutex);
		}
		i = buffer[next_available_index_cons];
		if (i != -1 && element_buffer > 0)
		{
			buffer[next_available_index_cons] = -1;
			element_buffer--;
			usage_count--;
			printf("Consumer used the value %d in the buffer\n", i);
		}
		next_available_index_cons = (next_available_index_cons + 1)%BUFFER_SIZE;
		pthread_mutex_unlock(&mutex);
		if (!usage_count)
		{
			exit(1);
		}
		pthread_cond_signal(&cond_pro);
	}
}