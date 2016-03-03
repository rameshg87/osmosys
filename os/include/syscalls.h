#ifndef SYSCALLS_DOT_H

#define SYSCALLS_DOT_H


#define LSHIFT 42
#define RSHIFT 54
#define CAPS 58
#define CTRL 29
#define ALT 56
#define ENTER 28
#define BACKSPACE 14

void printfhandler(struct reg_val *);
void scanfhandler(struct reg_val *);
void memoryhandler (struct reg_val *);
void filesystemhandler (struct reg_val *);
void processhandler (struct reg_val *);
void stringhandler (struct reg_val *);
void machine_functions (struct reg_val *);

#endif
