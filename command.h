#ifndef COMMAND_H
#define COMMAND_H

#define command(name) int name(int argc, char** argv);

int exists(char* cmdname);

command(cd)

#endif // command_h