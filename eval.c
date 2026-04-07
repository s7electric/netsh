#define _GNU_SOURCE // for fcntl changing pipe size to system max
#include "eval.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <fcntl.h>

#define try(func) if (-1 == (func)) {perror(#func); goto ERROR;}


char* eval(char* expr) {
	char* out = malloc(sizeof(char) * getPipeMax());
	/*
	1. tokenize expr
	2. if subevaluations are found then strip ?() and recurseively replace word with evaluation
	3. look for pipes etc ???
	4. return the evaluated line
	*/
	// 1
	int count;
	char** words = getwords(expr, &count, ' ');
	// 2: this is like the recursive case
	for (int i = 0; i < count; i++) {
		if (words[i][0] == EVALCHR) {
			char subexpr[strlen(words[i])-2]; // -2 for the ?() and enough for \0
			strncpy(subexpr, &words[i][2], strlen(words[i])-3); // strip ?()
			subexpr[strlen(words[i]) - 3] = '\n';
			subexpr[strlen(words[i]) - 2] = '\0';
			free(words[i]);
			words[i] = eval(subexpr);
			// ^ THIS MAY CAUSE PROBLEMS SINCE THE EVALUATION MIGHT RESULT IN MULTIPLE TOKENS, WHICH WILL NOT BE ACCOUNTED FOR
		}
	}

	// 3: we should have a string with no subevaluations left (this is like the base case) e.g. "ls | grep ... | wc"
	pipejobqueue* last = malloc(sizeof(pipejobqueue));
	pipejobqueue* first = last;
	first->fdin = STDIN_FILENO;
	first->prev = NULL;
	first->next = NULL;
	first->argc = 0;
	for (int i = 0; i < count; i++) {
		if (0 != strcmp(words[i], "|")) { // if not pipe
			last->argv[last->argc] = calloc((MAXARGLEN+1), sizeof(char));
			strncpy(last->argv[last->argc], words[i], MAXARGLEN);
			last->argv[last->argc][MAXARGLEN] = '\0';
			last->argc++;
		}
		// else if redirection operators TBD
		else { // if we encounter a pipe
			last->argv[last->argc] = NULL;
			// advance the queue pointer and assemble pipes
			int p[2];
			try(pipe(p))
			setPipeMax(p[1]);
			pipejobqueue* new = malloc(sizeof(pipejobqueue));
			new->fdin = p[0];
			new->prev = last;
			new->next = NULL;
			new->argc = 0;

			last->next = new;
			last->fdout = p[1];
			last = new;
		}
	}
	last->argv[last->argc] = NULL; // terminate final program argument list for exec()

	int mainpipe[2];
	try(pipe(mainpipe))
	last->fdout = mainpipe[1];
	// job fork sequence
	int pid;
	while (first != NULL) {
		try(pid = fork())
		if (pid == 0) { // child (actual program)

			// close all unused fds
			// try(close(mainpipe[0]))
			while (last != NULL) { // why did I implement this even; it's currently 9:14pm april 6th
				if (last == first) { // close all the pipe fds from only the other programs
				}
				else if (last->fdin == STDIN_FILENO) {
					if (-1 == close(last->fdout)) {
						fprintf(stderr, "here\n");
					}
				}
				else {
					if (-1 == close(last->fdin)) {
						fprintf(stderr, "here\n");
					}
					try(close(last->fdout))
				}
				last = last->prev;
			}
			try(dup2(first->fdin, STDIN_FILENO))
			try(dup2(first->fdout, STDOUT_FILENO))
			// execute this program in the chain
			execvp(first->argv[0], first->argv);
			exit(1);
		}
		first = first->next;
	}
	// close open pipe fds in parent
	while (last != NULL) { // walk back up the queue to close and free everything
		if (last->fdin != STDIN_FILENO) {
			try(close(last->fdin))
		}
		try(close(last->fdout))
		last = last->prev;
	}

	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			goto ERROR;
		}
	}
	// tcsetpgrp(STDIN_FILENO, getpgrp());
	// write results to output buffer
	if (out == NULL) return NULL;
	int bytestotal = 0;
	int bytesread;
	do {
		bytesread = read(mainpipe[0], out+bytestotal, 1024); // 1024 because I think it's reasonable
		if (bytesread == -1) {
			goto ERROR;
		}
		bytestotal += bytesread;
	}
	while (bytesread != 0);
	return out;
	ERROR:
	//free everything
	// free previous element FIX
	free(out);
	return NULL;
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