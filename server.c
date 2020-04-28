#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_FILE	"./fifo_temp"
#define BUF_SIZE	1337

int main(void)
{
	int cnt = 0;
	int fd;
	int score = 100;
	char buf[BUF_SIZE];
	char pingstr[] = "ping\n";
	sem_t* sem;

	/*
	 * To-do : initialize for game
	 * get named semaphore, fifo, etc. 
	 */
	mkfifo(FIFO_FILE, 0666);
	fd = open(FIFO_FILE, O_RDWR);
	
	if ((sem = sem_open("serversema", O_CREAT, 0644, 1)) == SEM_FAILED)
	{
		perror("open");
		exit(1);
	}
	sem_wait(sem);
	printf("Your turn!\n");
	memset(buf, 0x00, BUF_SIZE);
	fgets(buf, BUF_SIZE, stdin);
	write(fd,buf, strlen(buf));
	if (strcmp(buf, pingstr))
	{
		printf("wrong! %d\n", -20);
		score -= 20;
	}
	sem_post(sem);
	sleep(1);
	for (cnt=1; cnt<5; cnt++)
	{
		sem_wait(sem);
		read(fd, buf, BUF_SIZE);
		printf("[opponent] %s", buf);
		printf("Your turn!\n");
		memset(buf, 0x00, BUF_SIZE);
		/*
		 * To-do : complete game process
		 */
		fgets(buf, BUF_SIZE, stdin);
		write(fd, buf, strlen(buf));

		if (strcmp(buf, pingstr))
		{
			/*
			 * To-do : handle wrong case
			 */
			printf("wrong! %d\n", -20);
			score -= 20;
		}
		sem_post(sem);
		sleep(1);
	}

	printf("Done! Your score : %d\n", score);

	close (fd);

	return 0;
}
