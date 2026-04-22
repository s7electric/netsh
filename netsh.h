#ifndef NETSH_H
#define NETSH_H

#define MAXARGLEN 30
#define MAXARGS 15
#define MAXLINE ((MAXARGLEN+1) * MAXARGS) //+1 for spaces
#define EVALCHR '$'

#define SHELL_CMD_ERR 3

#define err(cause, str, info, GOTO) {perror(#cause); fprintf(stderr, str, info); goto GOTO;}

#endif // netsh_h