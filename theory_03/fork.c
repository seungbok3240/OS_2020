#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	pid_t pid;
	pid = fork();
	if(pid == 0) {
		printf("pid_is_zero_parent : %d\n", getppid());
		printf("pid_is_zero_own : %d\n", getpid());
	} else {
		printf("pid_nonzero_own : %d\n", getpid());
		printf("pid_nonzero_child : %d\n", pid);
		wait(pid);
	}
	return 0;
}
