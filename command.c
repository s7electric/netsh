#include "command.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define STB_DS_IMPLEMENTATION
#include "lib/stb_ds.h"

struct cmdlookup {
	char* key;
	int (*value)(int, char**);
};

static struct cmdlookup* cmdlookup = NULL;

void lookupTableInit() {
	shput(cmdlookup, "cd", cd);
	shput(cmdlookup, "exit", _exit_);
}

char exists(char* cmdname) {
	if (shgeti(cmdlookup, cmdname) == -1) {
		return 0;
	}
	else {
		return 1;
	}
}

void runcmd(int argc, char** argv) {
	shget(cmdlookup, argv[0])(argc, argv);
}

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

int _exit_(int argc, char** argv) {
	exit(0);
}