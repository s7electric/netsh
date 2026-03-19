#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"
#include "dir.h"
#define MAXARGLEN 30
#define MAXARGS 15
#define MAXLINE ((MAXARGLEN+1) * MAXARGS) //+1 for spaces

int main(int argc, char** argv) {

	char directory[256];
	if (-1 == pwd(directory, 256)) {
		fprintf(stderr, "Failed to obtain working directory");
	}

	int stop = 1;
	while (stop == 1) {
		printf("\n%s $>", directory);
		char input[MAXLINE];
		fgets(input, MAXLINE, stdin);
		if (!strcmp(input, "\n")) continue;
		int argc2;
		char** argv2 = getwords(input, &argc2);
		if (argv2 == NULL) {
			fprintf(stderr, "Failed to parse arguments");
			continue;
		}
		int proc = createProcess(argc2, argv2);
		if (proc == -1) {
			fprintf(stderr, "Failed to start process");
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

char** getwords(char* inputstr, int* count) {
	*count = 0;
	char** argv2 = malloc(sizeof(char*) * MAXARGS);

	int i = 0,arglen = 0;

	while (inputstr[i] != '\0' && *count <= MAXARGS && i < MAXLINE) {
		while (inputstr[i] != ' ' && inputstr[i] != '\n') {
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
		while (inputstr[i] == ' ') i++; //consume spaces between args
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