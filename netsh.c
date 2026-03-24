#include <bits/posix1_lim.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include "shell.h"

int main(int argc, char** argv) {

	char directory[PATH_MAX];
	char hostname[HOST_NAME_MAX];
	// if (-1 == pwd(directory, 256)) {
	// 	fprintf(stderr, "Failed to obtain working directory");
	// }

	int stop = 1;
	while (stop == 1) {
		if (NULL == getcwd(directory, sizeof(directory))) {
			perror("cwd");
		}
		if (-1 == gethostname(hostname, HOST_NAME_MAX)) {
			perror("gethostname");
		}
		printf("\n[%s] $>", directory);
		char input[MAXLINE];
		fgets(input, MAXLINE, stdin);
		if (!strcmp(input, "\n")) continue;
		int argc2;
		char** argv2 = getwords(input, &argc2);
		if (argv2 == NULL) {
			fprintf(stderr, "Failed to parse arguments");
			continue;
		}
		if (executeCommand(argc2, argv2, directory) == 0) {
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