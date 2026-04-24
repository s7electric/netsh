#ifndef COMMAND_H
#define COMMAND_H

#define command(name) int name(int argc, char** argv);

void lookupTableInit();

char exists(char* cmdname);

void runcmd(int argc, char** argv);

command(cd)
command(_exit_)

#endif // command_h