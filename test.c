#include <stdio.h>
#include "shell.h"

int main() {
    char buf[150];
    int argc;
    char delim = getchar();
    getchar();
    fgets(buf, 100, stdin);
    char** words = getwords(buf, &argc, delim);
    for (int i = 0; i < argc; i++) {
        printf("%s\n", words[i]);
    }
}