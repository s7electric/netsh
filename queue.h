#ifndef QUEUE_H
#define QUEUE_H

#ifndef NETSH_H
#include "netsh.h"
#endif

typedef struct pipejobqueue pipejobqueue;

pipejobqueue* createQueue(int fdoutfinal);

int enqueue(pipejobqueue* pq, int argc, char* argv[]);

int execute(pipejobqueue* pq);

int filein(pipejobqueue* pq, int fd);

int fileout(pipejobqueue* pq, int fd);

int closefdin(pipejobqueue* pq);

int freeQueue(pipejobqueue* pv);

#endif // queue_h