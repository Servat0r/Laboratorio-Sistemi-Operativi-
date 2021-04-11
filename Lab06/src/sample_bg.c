#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
	if (argc != 2){
		printf("Usage: %s <int(>0)>\n", argv[0]);
		return EXIT_FAILURE;
	}
	pid_t self_pid = getpid();
	pid_t parent_pid = getppid();
	pid_t pid = fork();
	if (pid == 0){
		execl("/bin/sleep", "/bin/sleep", argv[1], (char*)NULL);
		perror("sleeping");
		return EXIT_FAILURE;
	} else {
		int status;
		if (waitpid(pid, &status, 0) == -1){
			perror("waiting");
			return EXIT_FAILURE;
		}
		if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)){
			printf("My PID is: %d\nMy PPID is: %d\n", self_pid, parent_pid);
		} else {
			perror("awaken");
		}
		return 0;
	}
}
