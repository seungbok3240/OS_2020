#include <stdio.h>

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

	for (cnt=0; cnt<5; cnt++)
	{
		sem_wait(sem);
		printf("Your turn!\n");
		memset(buf, 0x00, BUF_SIZE);
		/*
		 * To-do : complete game process
		 */
		if (strcmp(buf, pingstr))
		{
			/*
			 * To-do : handle wrong case
			 */
		}
		sem_post(sem);
		sleep(1);
	}

	printf("Done! Your score : %d\n", score);

	close (fd);

	return 0;
}
