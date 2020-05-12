#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

const char *server_to_client = "server_to_client";
const char *client_to_server = "client_to_server";
const char *correct = "Correct";
const char *wrong = "Wrong";
const char *too_late = "Too late";

int main()
{
	int fd;
	int c_to_s;
	FILE* quizbox;
	char* addr;
	char* c_to_s_mmap;
	struct stat statbuf;
	struct stat c_to_s_statbuf;

	sem_t* sema_enterance;
	sem_t* sema_start;
	sem_t* sema_check;
	sem_t* sema_isCorrect;
	sem_t* sema_waitquiz;

	int client_count = 0;	//how many people in game
	
	/*
	 * wating 2 users
	 */
	sem_unlink("seam_enterance");
	if ((sema_enterance = sem_open("sema_enterance", O_CREAT, 0664, 0)) == SEM_FAILED) {
	perror("sema_enterance not open");
exit(1);
	}
	
	sem_unlink("sema_start");
	if ((sema_start = sem_open("sema_start", O_CREAT, 0664, 0)) == SEM_FAILED) {
		perror("sema_start error");
		exit(1);
	}
	
	sem_unlink("sema_check");
	if ((sema_check = sem_open("sema_check", O_CREAT, 0664, 0)) == SEM_FAILED) {
		perror("sema_check error");
		exit(1);
	}
	
	sem_unlink("sema_isCorrect");
	if ((sema_isCorrect = sem_open("sema_isCorrect", O_CREAT, 0664, 0)) == SEM_FAILED) {
		perror("sema_endgame error");
		exit(1);
	}
	
	sem_unlink("sema_waitquiz");
	if ((sema_waitquiz = sem_open("sema_waitquiz", O_CREAT, 0664, 0)) == SEM_FAILED) {
		perror("sema_waitquiz error");
		exit(1);
	}

	while(client_count != 2) {
		sem_wait(sema_enterance);
		printf("clinet %d hi\n", client_count);
		client_count += 1;
	}
	sem_close(sema_enterance);
	sem_unlink("sema_enterance");
	
	/* 
	 * Playing a Game
	 */
	sem_post(sema_start);
	sem_post(sema_start);
	sleep(2);
	/*
	 * Create Quiz and send to each client
	 */
	if ((quizbox = fopen("proverb.txt", "r")) == NULL) {
		perror("Sorry, don't load quizbox");
		exit(1);
	}
	
	char proverb_array[20][300];
	int proverb_index = 0;
	while(!feof(quizbox)) {
		char buffer[300];
		fgets(buffer, sizeof(buffer), quizbox);
		buffer[strlen(buffer) - 1] = '\0';
		strcpy(proverb_array[proverb_index], buffer);
		proverb_index += 1;
	}
	fclose(quizbox);
	if((fd = open(server_to_client, O_RDWR)) == -1) {
		perror("open error");
		exit(1);
	}
	
	if (fstat(fd, &statbuf) == -1) {
		perror("fstat error");
		exit(1);
	}
	
	if((c_to_s = open(client_to_server, O_RDWR)) == -1) {
		perror("client_to_server open error");
		exit(1);
	}

	if (fstat(c_to_s, &c_to_s_statbuf) == -1) {
		perror("c_to_s statbuf error");
		exit(1);
	}

	addr = (char*)mmap(NULL, statbuf.st_size , (PROT_READ|PROT_WRITE), MAP_SHARED, fd, (off_t)0);
	
	c_to_s_mmap = (char*)mmap(NULL, c_to_s_statbuf.st_size, (PROT_READ|PROT_WRITE), MAP_SHARED, c_to_s, (off_t)0);

	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	
	srand(time(NULL));
	
	int quiz_index = rand() % 21;
	memcpy(addr, proverb_array[quiz_index], sizeof(proverb_array[quiz_index]));
	sem_post(sema_waitquiz);
	sem_post(sema_waitquiz);

	printf("Prob : %s\n", addr);
	printf("===================================\n");
	printf("\tGame Start\n");
	printf("===================================\n");
	
	int flag = 0;
	while(1) {
		sem_wait(sema_check);
		char user_answer[300];
		memcpy(user_answer, c_to_s_mmap, sizeof(user_answer));
	
		int iswinner = strncmp(user_answer, proverb_array[quiz_index], 20);
		if ((!iswinner) && flag == 0) {
			memcpy(addr, "", sizeof(user_answer));
			memcpy(addr, correct, sizeof(correct));
			flag = 1;
		}
		else if (flag == 1){
			memcpy(addr, "", sizeof(user_answer));
			memcpy(addr,too_late, sizeof(too_late));
		}
		else {
			memcpy(addr, "", sizeof(user_answer));
			memcpy(addr, wrong, sizeof(wrong));
		}
		sem_post(sema_isCorrect);
		sleep(1);

		printf("%s\n", c_to_s_mmap);
	}
	
	close(fd);
	
	if(munmap(addr, statbuf.st_size) == -1) {
		perror("munmap");
		exit(1);
	}
	
	return 0;
}
