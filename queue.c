#include "netsh.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct pipejobqueue {
    struct job* ready;
    struct job* end;
    int mainpipe[2];
};

struct job {
    int fdin;
    int fdout;
    int argc;
    struct job* next;
    struct job* prev;
    char** argv;
};

pipejobqueue* createQueue(int mainpipe[2]) {
    pipejobqueue* pq = malloc(sizeof(pipejobqueue));
    if (!pq) {perror("malloc"); return NULL;}
    pq->end = NULL;
    pq->ready = NULL;
    pq->mainpipe[0] = mainpipe[0];
    pq->mainpipe[1] = mainpipe[1];
    return pq;
}

int enqueue(pipejobqueue* pq, int argc, char** argv) {
    struct job* jb = malloc(sizeof(struct job));
    if (!jb) goto FREE_JOB;

    jb->argc = argc;
    jb->argv = malloc(sizeof(char*) * (argc+1));
    if (!jb->argv) goto FREE_ARGV;

    // allocate space for arguments
    for (int i = 0; i < argc; i++) {
        jb->argv[i] = argv[i];
    }
    
    jb->argv[argc] = NULL;
    jb->fdout = pq->mainpipe[1];
    jb->next = NULL;

    if (pq->ready == NULL) { // this is the first job
        pq->ready = jb;
        pq->end = jb;
        jb->fdin = STDIN_FILENO;
    }
    else { // this is not the first job
        pq->end->fdout = -1;

        jb->prev = pq->end;
        pq->end->next = jb;
        pq->end = jb;
    }
    return 0;

    FREE_ARGV:
    free(jb->argv);

    FREE_JOB:
    free(jb);
    fprintf(stderr, "failed to allocated memory for job %s\n", argv[0]);
    return MEM_ERR;
}

int executejob(pipejobqueue* pq) {
    if (!pq->ready) goto EMPTY_FAIL;

    int p[2];
    if (-1 == pipe(p)) err(pipe, "pipe %c", '\n', PIPE_FAIL)
    if (pq->ready->next) {
        pq->ready->fdout = p[1];
        pq->ready->next->fdin = p[0];
    }

    int pid = fork();
    if (pid == 0) {
        if (-1 == close(p[0])) err(close, "close %d\n", p[0], CLOSE_FAIL)
        if (-1 == close(pq->mainpipe[0])) err(close, "close %d\n", pq->mainpipe[0], CLOSE_FAIL)
        if (pq->ready->next) {
            if (-1 == close(pq->mainpipe[1])) err(close, "close %d\n", pq->mainpipe[1], CLOSE_FAIL)
        }
        else {
            if (-1 == close(p[1])) err(close, "close %d\n", p[1], CLOSE_FAIL)
        }

        dup2(pq->ready->fdin, STDIN_FILENO);
        // if (pq->ready->fdin != STDIN_FILENO) {
        //     if (-1 == close(pq->ready->fdin)) err(close, "close %d\n", pq->ready->fdin, CLOSE_FAIL)
        // }
        dup2(pq->ready->fdout, STDOUT_FILENO);
        // if (pq->ready->fdout != STDOUT_FILENO) {
        //     if (-1 == close(pq->ready->fdout)) err(close, "close %d\n", pq->ready->fdout, CLOSE_FAIL)
        // }

        execvp(pq->ready->argv[0], pq->ready->argv);
        exit(1);
    }
    else if (pid > 0) {
        if (pq->ready->fdin != STDIN_FILENO) {
            if (-1 == close(pq->ready->fdin)) err(close, "close %d\n", pq->ready->fdin, CLOSE_FAIL)
        }
        if (-1 == close(pq->ready->fdout)) err(close, "close %d\n", pq->ready->fdout, CLOSE_FAIL)
    }
    else err(fork, "forked %s", pq->ready->argv[0], FORK_FAIL);

    struct job* temp = pq->ready;
    pq->ready = pq->ready->next;
    free(temp->argv);
    free(temp);
    return pid;

    FORK_FAIL:
    return FORK_ERR;

    CLOSE_FAIL:
    return CLOSE_ERR;

    PIPE_FAIL:
    return PIPE_ERR;

    EMPTY_FAIL:
    return EMPTY_ERR;
}

int freequeue(pipejobqueue* pq) {
    free(pq);
    return 0;
}