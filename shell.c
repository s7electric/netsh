#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"
#define MAXARGLEN 30
#define MAXARGS 15
#define MAXLINE ((MAXARGLEN+1) * MAXARGS) //+1 for spaces

int main(int argc, char** argv) {
	int stop = 1;
	while (stop == 1) {
		printf("\n$>");
		char input[MAXLINE];
		fgets(input, MAXLINE, stdin);
		int argc2;
		char** argv2 = getargs(input, &argc2);
		if (argv2 == NULL) {
			fprintf(stderr, "Failed to parse arguments");
		}
		int proc = createProcess(argc2, argv2);
		if (proc == -1) {
			fprintf(stderr, "Failed to create child process");
			continue;
		}
		// for (int i = 0; i < argc2; i++) {
		// 	printf("arg%d: %s\n", i, argv2[i]);
		// }
		//free everything for next prompt
		for (int i = 0; i < argc2; i++) {
			free(argv2[i]);
		}
		free(argv2);
	}
}

char** getargs(char* input, int* argc) {
	*argc = 0;
	char** argv2 = malloc(sizeof(char*) * MAXARGS);

	int i = 0,arglen = 0;

	while (input[i] != '\0' && *argc <= MAXARGS && i < MAXLINE) {
		while (input[i] != ' ' && input[i] != '\n') {
			i++;
			arglen++;
		}
		argv2[*argc] = malloc(sizeof(char) * (arglen+1));
		for (int k = 0; k < arglen; k++) {
			argv2[*argc][k] = input[i-arglen+k];
		}
		argv2[*argc][arglen] = '\0';
		arglen = 0;
		*argc += 1;
		while (input[i] == ' ') i++; //consume spaces between args
		if (input[i] == '\n') break; //remove this later to add multiline commands
	}
	return argv2;
}

int createProcess(int argc, char** argv) {
	int status;
	int pid = fork();
	if (pid == -1) {
		perror("fork");
		return -1;
	}
	if (pid > 0) {
		wait(&status); //what
	}
	else if (pid == 0) {
		execvp(argv[0], argv);
		exit(0);
	}
	return status;
}