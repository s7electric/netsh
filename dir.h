#ifndef DIR_H
#define DIR_H

/*
Get the absolute path to the current directory (calls pwd) and store the result in dir with a max length of len
-> 0 if normal, 1 if error
*/
int pwd(char* dir, int len);

#endif