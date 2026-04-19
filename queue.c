#include "netsh.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define err(cause, GOTO) {perror(#cause); goto GOTO;}

struct pipejobqueue {
    struct job* ready;
    struct job* end;
    int fdoutfinal;
};

struct job {
    int fdin;
    int fdout;
    int argc;
    struct job* next;
    struct job* prev;
    char** argv;
};

pipejobqueue* createQueue(int fdoutfinal) {
    pipejobqueue* pq = malloc(sizeof(pipejobqueue));
    if (!pq) {perror("malloc"); return NULL;}
    pq->end = NULL;
    pq->ready = NULL;
    pq->fdoutfinal = fdoutfinal;
    return pq;
}

int enqueue(pipejobqueue* pq, int argc, char* argv[]) {
    struct job* jb = malloc(sizeof(struct job));
    if (!jb) goto FREE_JOB;

    jb->argc = argc;
    jb->argv = malloc(sizeof(char*) * (argc+1));
    if (!jb->argv) goto FREE_ARGV;

    // allocate space for arguments
    for (int i = 0; i < argc; i++) {
        jb->argv[i] = malloc(sizeof(char) * MAXARGLEN);
        if (!jb->argv[i]) goto FREE_WORDS;
        strncpy(jb->argv[i], argv[i], MAXARGLEN);
    }
    jb->argv[argc] = NULL;

    if (pq->ready == NULL) { // this is the first job
        pq->ready = jb;
        pq->end = jb;
        jb->fdin = STDIN_FILENO;
        jb->fdout = pq->fdoutfinal;
    }
    else { // this is not the first job
        int p[2];
        if (-1 == pipe(p)) {
            perror("pipe");
            goto PIPE_FAIL;
        }
        // update file descriptors
        pq->end->fdout = p[1];
        jb->fdin = p[0];
        jb->fdout = pq->fdoutfinal;

        // update pointers so jb is at the end
        jb->prev = pq->end;
        pq->end->next = jb;
        pq->end = jb;
    }
    return 0;

    FREE_WORDS:
    for (int i = 0; jb->argv[i] != NULL; i++) {
        free(jb->argv[i]);
    }

    FREE_ARGV:
    free(jb->argv);

    FREE_JOB:
    free(jb);
    fprintf(stderr, "failed to allocated memory for job %s", argv[0]);
    return MEM_ERR;

    PIPE_FAIL:
    return PIPE_ERR;
}

int executejob(pipejobqueue* pq) {
    if (!pq->ready) goto EMPTY_FAIL;
    int pid = fork();
    if (pid == 0) {
        /* we walk up from the end for no particular reason:
        we just use the existing reference chain to our advantage */
        while (pq->end) {
            if (pq->end != pq->ready) {
                close(pq->end->fdin);
                close(pq->end->fdout);
            }
            pq->end = pq->end->prev;
        }

        dup2(pq->ready->fdin, STDIN_FILENO);
        dup2(pq->ready->fdout, STDOUT_FILENO);

        execvp(pq->ready->argv[0], pq->ready->argv);
        exit(1);
    }
    else if (pid > 0) {
        close(pq->ready->fdin);
        close(pq->ready->fdout);
    }
    else goto FORK_FAIL;

    pq->ready = pq->ready->next;
    // free job?
    return pid;

    FORK_FAIL:
    return FORK_ERR;

    EMPTY_FAIL:
    return EMPTY_ERR;
}

int freeQueue(pipejobqueue* pv);