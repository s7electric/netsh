#ifndef EVALH
#define EVALH

#define MAXARGLEN 30
#define MAXARGS 15
#define MAXLINE ((MAXARGLEN+1) * MAXARGS) //+1 for spaces
#define EVALCHR '$'

typedef struct pipejobqueue {
    int fdin;
    int fdout;
    int argc;
    char* argv[MAXARGS];
    struct pipejobqueue * next;
    struct pipejobqueue * prev;
} pipejobqueue;


/*
Create a list of individual space separated words, and store number of words in *count. Call freewords after use.
$(...) is treated as a single token.
inputstr: string to tokenize.
count: pointer to the wordcount generated on return.
delim: delimiter to tokenize.
-> *count length array of strings, NULL on malform error.
*/
char** getwords(char* inputstr, int* count, char delim);

/*
Free the string array returned by getwords.
*/
void freewords(char** wordlist, int len);

/*
Perform in-text replacement by evaluating the shell input.
expr: string to evaluate as if it had been typed into the shell.
-> evaluated string as if it had come to stdout (must be freed)
*/
char* eval(char* expr);

/*
Set the size of pipe fd to the system maximum.
-> -1 on error
*/
int setPipeMax(int fd);

/*
Get the system maximum size of a pipe.
-> -1 on error
*/
int getPipeMax();


#endif