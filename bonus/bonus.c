#include <stdio.h>
#include <unistd.h>

int main(void)
{
	char *path = "/bin/dmesg";
	char *argv[] = {"/bin/dmesg", NULL, NULL};
	char *envp[] = {NULL};

	execve(path, argv, envp);

	printf("hello\n");

	return 0;
}
