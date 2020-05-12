#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

const char *client_to_server = "client_to_server";
const char *server_to_client = "server_to_client";
const char *correct = "Correct";
const char *too_late = "Too late@";

int main()
{	
	int c_to_s;
	int s_to_c;
	char* c_to_s_mmap;
	char* s_to_c_mmap;
	struct stat statbuf;
	struct stat s_to_c_statbuf;
	sem_t* sema_enterance;
	sem_t* sema_start;
	sem_t* sema_check;
	sem_t* sema_isCorrect;
	sem_t* sema_waitquiz;

	if ((sema_enterance = sem_open("sema_enterance", 0, 0664, 0)) == SEM_FAILED) {
		printf("You can't enter this room\n");
		exit(1);
	}
	sem_post(sema_enterance);
	sleep(2);

	if ((sema_start = sem_open("sema_start", 0664, 0)) == SEM_FAILED) {
			perror("sema_start error");
			exit(1);
	}
	printf("wating other user\n");
	sem_wait(sema_start);
	
	if ((sema_check = sem_open("sema_check", 0664, 0)) == SEM_FAILED) {
		perror("sema_check error");
		exit(1);
	}
	
	if ((sema_isCorrect = sem_open("sema_isCorrect", 0664, 0)) == SEM_FAILED) {
	perror("sema_isCorrect error");
	exit(1);
	}
	
	if ((sema_waitquiz = sem_open("sema_waitquiz", 0664, 0)) == SEM_FAILED) {
		perror("sema_waitquiz error");
		exit(1);
	}

	int pid = getpid();
	printf("================================\n");
	printf("\tHello! %d!\n", pid);
	printf("================================\n");

	if((c_to_s = open(client_to_server, O_RDWR)) == -1) {
	perror("client_to_server not open");
exit(1);
	}
	if((s_to_c = open(server_to_client, O_RDWR)) == -1) {
		perror("server_to_client not open");
		exit(1);
	}

	if (fstat(c_to_s, &statbuf) == -1) {
	perror("stat error");
exit(1);
	}
	
	if (fstat(s_to_c, &s_to_c_statbuf) == -1) {
		perror("s_to_c stat error");
		exit(1);
	}

	c_to_s_mmap =(char*)mmap(NULL, statbuf.st_size, (PROT_READ|PROT_WRITE), MAP_SHARED, c_to_s, (off_t)0);
	
	s_to_c_mmap = (char *)mmap(NULL,s_to_c_statbuf.st_size, (PROT_READ|PROT_WRITE), MAP_SHARED, s_to_c, (off_t)0);

	if (c_to_s_mmap == MAP_FAILED) {
		perror("addr mmap error");
		exit(1);
	}
	
	if (s_to_c_mmap == MAP_FAILED) {
		perror("s_to_c_mmap error");
		exit(1);
	}

	char user_input[300];
	char str_pid[10];
	sprintf(str_pid, "%d", pid);
	
	sem_wait(sema_waitquiz);
	printf("Proverb : %s\n", s_to_c_mmap);
	while(1) {
		char send_message[300] = "[User ";
		printf(">>");
		gets(user_input);
		memcpy(c_to_s_mmap, "", sizeof(user_input));
		memcpy(c_to_s_mmap, user_input, sizeof(user_input));
		sem_post(sema_check);
		sem_wait(sema_isCorrect);
		
		strcat(send_message, str_pid);
		strcat(send_message, "]");
		strcat(send_message, s_to_c_mmap);
		strcat(send_message, " / Data : ");
		strcat(send_message, user_input);
		memcpy(c_to_s_mmap, send_message, sizeof(send_message));
		printf("%d %s\n",pid, s_to_c_mmap);
		if (strcmp(s_to_c_mmap, correct) == 0) {
			break;
		}

		if (strcmp(s_to_c_mmap, too_late) == 0) {
			break;
		}
	}

	close(c_to_s);
	close(s_to_c);
	return 0;
}
