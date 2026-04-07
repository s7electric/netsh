#include "eval.h"
#include "command.h"
#include <bits/posix1_lim.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {

	char directory[PATH_MAX];
	char hostname[HOST_NAME_MAX];
	int stop = 1;
	while (stop == 1) {
		if (NULL == getcwd(directory, sizeof(directory))) {
			perror("getcwd");
		}
		if (-1 == gethostname(hostname, HOST_NAME_MAX)) {
			perror("gethostname");
		}
		printf("[%s][%s] %c>", hostname, directory, EVALCHR);

		char input[MAXLINE];
		fgets(input, MAXLINE, stdin);
		if (!strcmp(input, "\n")) continue; // expand this to actually work based on real functionality and not special cases
		int argc2;

		input[strlen(input)] = '\0';
		char* output = eval(input);
		if (output != NULL) {
			printf("%s", output);
		}
		free(output);
	}
}