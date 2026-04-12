#include "command.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

int executeCommand(int argc, char **argv) {
	if (!strcmp(argv[0], "cd")) {
		if (argc == 1) {
			//handle cd to home
			return 0;
		}
		else if (argc == 2) {
			if (-1 == chdir(argv[1])) {
				perror("chdir");
				return -1;
			}
			return 0;
		}
		else {
			fprintf(stderr, "cd: usage: cd [directory]");
			return -1;
		}
	}
	// fprintf(stderr, "netsh: command %s not found", argv[0]);
	return -1;
}