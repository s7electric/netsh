#ifndef COMMAND_H
#define COMMAND_H

/*
Check the cmdline argument list for built in shell commands.
-> 0 if success, 1 if failure, -1 if error
*/
int executeCommand(int argc, char** argv);

#endif // command_h