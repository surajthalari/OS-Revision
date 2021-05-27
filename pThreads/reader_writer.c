#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#define READER_COUNT 5
#define WRITER_COUNT 5

char * file_path;
int resource_counter = 0;

pthread_cond_t read_phase = PTHREAD_COND_INITIALIZER;
pthread_cond_t write_phase = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *reader(void *args);
void *writer(void *args);

char* get_path();
void read_file();
void write_file();
int count_lines();

int main(int argc, char const *argv[])
{
	file_path = get_path();

	pthread_t ThreadR[READER_COUNT], ThreadW[WRITER_COUNT];
	int i=READER_COUNT;
	int j = WRITER_COUNT;
	while (j--)
	{
		if (pthread_create(&ThreadW[i], NULL, &writer, NULL))
		{	
			printf("Writer Thread failed to execute.\n");
		}	
	}

	while(i--){
		if (pthread_create(&ThreadR[i], NULL, &reader, NULL) != 0)
		{
			printf("Reader Thread %d\n failed to execute.", i);
		}
	}

	j = WRITER_COUNT;
	while (j--)
	{
		pthread_join(ThreadW[j], NULL);
	}

	i = READER_COUNT;
	while(i--){
		pthread_join(ThreadR[i], NULL);
	}

	return 0;
}

void *reader(void *args){
	
	pthread_mutex_lock(&mutex);
	if (resource_counter == -1)
	{
		pthread_cond_wait(&read_phase, &mutex);
	}
	resource_counter++;
	pthread_mutex_unlock(&mutex);

	//Reading file
	read_file();

	pthread_mutex_lock(&mutex);
	resource_counter--;
	pthread_mutex_unlock(&mutex);
	if (resource_counter == 0)
	{
		pthread_cond_broadcast(&write_phase);
	}
}

void *writer(void *args){
	pthread_mutex_lock(&mutex);
	if (resource_counter != 0)
	{
		pthread_cond_wait(&write_phase, &mutex);
	}
	resource_counter = -1;
	pthread_mutex_unlock(&mutex);

	//Writing file
	write_file();

	pthread_mutex_lock(&mutex);
	resource_counter = 0;
	pthread_cond_broadcast(&read_phase);
	pthread_cond_broadcast(&write_phase);
	pthread_mutex_unlock(&mutex);
}

void read_file(){
	printf("#####################read####################\n");
	int count = count_lines();
	printf("There are %d lines in the file\n", count);
}

void write_file(){
	FILE *wptr = fopen(file_path, "a");
	if (wptr == NULL)
	{
		perror("Unable to open the file");
		exit(1);
	}
	printf("#################write#######################\n");
	char c;
	int count = 0;
	for (c = getc(wptr); c != EOF; c = getc(wptr)){
        if (c == '\n'){
        	count = count + 1;
        }     
	}
	char *data = (char*) malloc(7*sizeof(char));
	strcpy(data, "Line : ");
	char number = count + 1 + '0';
	strncat(data, &number, 1);
	fprintf(wptr, data);
	fprintf(wptr, "\n");
	printf("A line is written to the file\n");
	printf("Now the total lines are %d\n", count + 1);
	fclose(wptr);
}

int count_lines(){
	FILE *fptr = fopen(file_path, "r");
	if (fptr == NULL)
	{
		perror("Unable to open the file");
		exit(1);
	}
	char c;
	int count = 0;
	for (c = getc(fptr); c != EOF; c = getc(fptr)){
        if (c == '\n'){
        	count = count + 1;
        }     
	}
	fclose(fptr);
	return count;
}

char* get_path(){
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("Current Directory is : %s\n", cwd);
	}
	else
	{
		perror("getcwd() error");
		exit(1);
	}

	char * file = "/reader_writer.txt";
	char * file_path = (char *)malloc(1 + strlen(cwd) + strlen(file));

	if (file_path == NULL)
	{
		perror("Out of Memory");
		exit(1);
	}

	strcpy(file_path, cwd);
	strcat(file_path, file);

	if ( access(file_path, F_OK) != 0 )
	{
		printf("File %s does not exist in the folder\n", file);
		printf("%s\n", file_path);
		FILE *fptr = fopen(file_path, "w+");
		if (fptr == NULL)
		{
			perror("unable to create the file");
			exit(1);
		}
		printf("File %s is created\n", file);
		fclose(fptr);
	}
	else
	{
		printf("File %s exists in the folder\n", file);
	}
	return file_path;
}