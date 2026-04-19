#include "netsh.h"
#include "eval.h"
#include "command.h"

#include <bits/posix1_lim.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define STB_DS_IMPLEMENTATION
#include "lib/stb_ds.h"

int main(int argc, char** argv) {
	/* Initialize built-in shell command lookup table */
	Cmdtable *cmdlookup = NULL;
	shput(cmdlookup, "cd", cd);


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

		fflush(stdout);
		printf("[%s][%s] %c>", hostname, directory, EVALCHR);
		
		char* expr = malloc(sizeof(char) * MAXLINE);
		fgets(expr, MAXLINE, stdin);

		int status, pid;
		if (0 == (pid = fork())) {
			if (!strcmp(expr, "\n")) continue; // expand this to actually work based on real functionality and not special cases

			int err = eval(&expr);

			if (err == 0) {
				printf("%s", expr);
				exit(0);
			}
			else if (err == SHELL_CMD_ERR) exit(SHELL_CMD_ERR);
			else exit(1);
		}
		else if (pid < 0) {
			perror("fork");
			fprintf(stderr, "netsh: process limit reached\n");
		}
		waitpid(pid, &status, 0);

		if (status == SHELL_CMD_ERR) {
			int cmdargc;
			char** cmdargv = getwords(expr, &cmdargc, ' ');
			int commandloc = executeCommand(cmdargc, cmdargv);
			cmdlookup[commandloc].value(cmdargc, cmdargv);
			freewords(cmdargv, cmdargc);
		}
	}
}