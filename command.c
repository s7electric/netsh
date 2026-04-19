#include "command.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

int cd(int argc, char** argv) {
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