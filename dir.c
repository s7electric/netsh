#include "dir.h"
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#define try(reason) if (-1 == reason) {perror(#reason); return 1;}


int pwd(char* directory, int len) {
    int p[2];
    try(pipe(p))
    int status;

    int forkpid = fork();
    try(forkpid)
    else if (0 == forkpid) {
        try(close(p[0]))
        try(dup2(p[1], STDOUT_FILENO))
        try(execlp("pwd", "pwd", NULL))
    }
    else if (0 < forkpid) {
        try(close(p[1]))
        try(wait(&status))
        if (WIFEXITED(status)) {
            try(WEXITSTATUS(status))
            try(read(p[0], directory, len));
            char* c = directory;
            while (*c != '\n') c++;
            *c = '\0';
        }
    }
    return 0;
}