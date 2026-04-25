#include "netsh.h"
#include "eval.h"
#include "command.h"

#include <bits/posix1_lim.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv) {
	/* Initialize built-in shell command lookup table */
	lookupTableInit();

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
		
		char* expr = malloc(sizeof(char) * MAXLINE);
		fgets(expr, MAXLINE, stdin);

		if (!strcmp(expr, "\n")) continue; // expand this to actually work based on real functionality and not special cases

		int err = eval(&expr);

		if (err == SHELL_CMD_ERR) {
			int cmdargc;
			char** cmdargv = getwords(expr, &cmdargc, ' ');

			runcmd(cmdargc, cmdargv);

			freewords(cmdargv, cmdargc);
		}
		printf("%s\n", expr);

		free(expr);
		expr = NULL;
		// sleep(1);
	}
}