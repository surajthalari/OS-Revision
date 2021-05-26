#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#define READER_COUNT 10

FILE *fptr;
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
	file_path = getcwd();

	pthread_t ThreadR[READER_COUNT], ThreadW;
	int i=READER_COUNT;

	while(i--){
		if (pthread_create(&ThreadR[i], NULL, &reader, NULL) != 0)
		{
			printf("Reader Thread %d\n failed to execute.", i);
		}
	}

	if (pthread_create(&ThreadW, NULL, &writer, NULL))
	{
		printf("Writer Thread failed to execute.\n");
	}

	i = READER_COUNT;
	while(i--){
		pthread_join(ThreadR[i], NULL);
	}
	pthread_join(ThreadW, NULL);

	return 0;
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
		fptr = fopen(file_path, "w+");
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

int count_lines(){
	fptr = fopen(file_path, "r");
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