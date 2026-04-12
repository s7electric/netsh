#ifndef EVAL_H
#define EVAL_H

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
*expr will be freed and **expr will point to the result of the evaluation.
expr: address of string to evaluate as if it had been typed into the shell.
*/
void eval(char** expr);

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


#endif // eval_h