#ifndef SHELLH
#define SHELLH

#define MAXARGLEN 30
#define MAXARGS 15
#define MAXLINE ((MAXARGLEN+1) * MAXARGS) //+1 for spaces

/*
Check the cmdline argument list for built in shell commands from the given directory.
-> 0 if success or error, 1 if failure
*/
int executeCommand(int argc, char** argv, char* directory);

/*
Create a list of individual space separated words from input and store count in *count.
THE ARRAY AND EACH ELEMENT MUST BE FREED
-> *count length array of strings.
*/
char** getwords(char* inputstr, int* count, char delim);

/*
Create a child process from the shell with argc-many arguments argv.
-> exit status of child process.
*/
int createProcess(int argc, char** argv);


#endif