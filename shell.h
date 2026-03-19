#ifndef SHELLH
#define SHELLH

char* choosearg(char*);

/*
Create a list of individual space separated words from input and store count in *count.
-> *count length array of strings.
*/
char** getwords(char* inputstr, int* count);

/*
Create a child process from the shell with argc-many arguments argv.
-> exit status of child process.
*/
int createProcess(int argc, char** argv);


#endif