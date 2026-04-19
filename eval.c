#include "netsh.h"
#include "eval.h"
#include "queue.h"
#include "command.h"

#define _GNU_SOURCE // for fcntl changing pipe size to system max

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <fcntl.h>


#define try(func) if (-1 == (func)) {perror(#func); goto ERROR;}
#define err(cause) {perror(#cause); goto ERROR;}


void eval(char** expr) {
	char* out = malloc(sizeof(char) * getPipeMax());
	/*
	1. tokenize expr
	2. if subevaluations are found then strip ?() and recurseively replace word with evaluation
	3. look for pipes etc ???
	4. return the evaluated line
	*/
	// 1
	int count;
	char** words = getwords(*expr, &count, ' ');
	// 2: this is like the recursive case
	for (int i = 0; i < count; i++) {
		if (words[i][0] == EVALCHR) {
			char subexpr[strlen(words[i])-2]; // -2 for the ?() and enough for \0
			strncpy(subexpr, &words[i][2], strlen(words[i])-3); // strip ?()
			// subexpr[strlen(words[i]) - 3] = '\n';
			subexpr[strlen(words[i]) - 2] = '\0';
			eval(&words[i]);
			// ^ THIS MAY CAUSE PROBLEMS SINCE THE EVALUATION MIGHT RESULT IN MULTIPLE TOKENS, WHICH WILL NOT BE ACCOUNTED FOR
		}
	}

	// try built-in commands
	if (0 == executeCommand(count, words)) {
		freewords(words, count);
		return;
	}

	// 3: we should have a string with no subevaluations left (this is like the base case) e.g. "ls | grep ... | wc > file"
	int mainpipe[2];
	try(pipe(mainpipe))
	pipejobqueue* pq = createQueue(mainpipe[1]);
	char* tempargs[MAXARGS];
	int argcounter = 0;
	for (int i = 0; i < count; i++) {
		if (0 != strcmp(words[i], "|")) { // if not pipe
			tempargs[argcounter] = words[i];
			argcounter++;
		}
		// else if redirection operators TBD
		else { // if pipe
			tempargs[argcounter] = NULL;
			if (-1 == enqueue(pq, argcounter, tempargs)) goto ERROR;
			argcounter = 0;
		}
	}
	tempargs[argcounter] = NULL;
	if (-1 == enqueue(pq, argcounter, tempargs)) goto ERROR;

	// job fork sequence
	int pid = 0;
	while (0 == (pid = executejob(pq)));

	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			goto CHILD_ERROR;
		}
	}
	// write results to output buffer
	int bytestotal = 0;
	int bytesread;
	do {
		bytesread = read(mainpipe[0], out+bytestotal, 1024); // 1024 because I think it's reasonable
		if (bytesread == -1) err(read);
		bytestotal += bytesread;
	}
	while (bytesread != 0);
	
	try(close(mainpipe[0]))

	// End: reallocate string
	char* p = *expr; // temp
	*expr = out;
	free(p);
	CHILD_ERROR:
	fprintf(stderr, "child exited with status: %d\n", WEXITSTATUS(status));
	ERROR:
	// free everything
	// free previous element FIX
	return;
}

int getPipeMax() {
	FILE* f = fopen("/proc/sys/fs/pipe-max-size", "r");
	if (f == NULL) return -1;
	int max;
	fscanf(f, "%d", &max);
	if (-1 == fclose(f)) {
		return -1;
	}
	return max;
}

int setPipeMax(int fd) {
	int max = getPipeMax();
	if (max == -1) return -1;
	int ret = fcntl(fd, F_SETPIPE_SZ, max);
	return ret;
}

void freewords(char** wordlist, int len) {
	for (int i = 0; i < len; i++) {
		free(wordlist[i]);
	}
	free(wordlist);
}

char** getwords(char* inputstr, int* argc, char delim) { // respects ?(...) as a single word
	*argc = 0;
	char** argv = malloc(sizeof(char*) * MAXARGS);

	int i = 0; // current place in string
	int arglen = 0; // length of current argument

	while (inputstr[i] != '\0' && *argc <= MAXARGS && i < MAXLINE) { // word loop
		while (inputstr[i] != delim && inputstr[i] != '\n') { // consume all characters
			if (inputstr[i] == EVALCHR) { // if ?( is found then jump to )
				if (arglen != 0 || inputstr[i+1] != '(') { // only tokenize this if ?() is well formed
					freewords(argv, *argc);
					return NULL;
				}
				char* end = strchr(&inputstr[i+1], ')');
				arglen = end - &inputstr[i] + 1;
				i = end - inputstr + 1;
				break;
			}
			i++;
			arglen++;
		}
		argv[*argc] = malloc(sizeof(char) * (arglen+1));
		for (int k = 0; k < arglen; k++) {
			argv[*argc][k] = inputstr[i-arglen+k];
		}
		argv[*argc][arglen] = '\0';
		arglen = 0;

		*argc += 1;
		while (inputstr[i] == delim) i++; //consume spaces between args
		if (inputstr[i] == '\n') break; //remove this later to add multiline commands
	}
	return argv;
}