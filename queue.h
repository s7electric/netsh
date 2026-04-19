#ifndef QUEUE_H
#define QUEUE_H

#define MEM_ERR -2
#define PIPE_ERR -3
#define FORK_ERR -4
#define EMPTY_ERR -5

typedef struct pipejobqueue pipejobqueue;

pipejobqueue* createQueue(int fdoutfinal);

int enqueue(pipejobqueue* pq, int argc, char* argv[]);

int executejob(pipejobqueue* pq);

#endif // queue_h