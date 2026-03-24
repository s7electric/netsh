#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include "shell.h"

char** getwords(char* inputstr, int* count, char delim) {
	*count = 0;
	char** argv2 = malloc(sizeof(char*) * MAXARGS);

	int i = 0,arglen = 0;

	while (inputstr[i] != '\0' && *count <= MAXARGS && i < MAXLINE) {
		while (inputstr[i] != delim && inputstr[i] != '\n') {
			i++;
			arglen++;
		}
		argv2[*count] = malloc(sizeof(char) * (arglen+1));
		for (int k = 0; k < arglen; k++) {
			argv2[*count][k] = inputstr[i-arglen+k];
		}
		argv2[*count][arglen] = '\0';
		arglen = 0;


		*count += 1;
		while (inputstr[i] == delim) i++; //consume spaces between args
		if (inputstr[i] == '\n') break; //remove this later to add multiline commands
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
		if (-1 == execvp(argv[0], argv)) {
			perror("execvp");
			return -1;
		}
		exit(0);
	}
	return status;
}

int executeCommand(int argc, char **argv, char* directory) {
	//fix this later
	if (!strcmp(argv[0], "cd")) {
		if (argc == 1) {
			//handle cd to home
			return 0;
		}
		else if (argc == 2) {
			if (-1 == chdir(argv[1])) {
				perror("chdir");
				return 1;
			}
			return 0;
		}
		else {
			fprintf(stderr, "cd: usage: cd [directory]");
			return 0;
		}
	}
	return 1;
}