#ifndef SYSCALL_H
#define SYSCALL_H

struct vm;
void syscall_print(struct vm *vm);
void syscall_input(struct vm *vm);
#endif