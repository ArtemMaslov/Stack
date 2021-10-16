#ifndef STACK_DIAGNOSTIC_H_
#define STACK_DIAGNOSTIC_H_


#include "Stack.h"


bool IsStackEmpty(Stack *stack);

void ClearStack(Stack *stack);

bool CheckForError(Stack* stack, StackErrors error);

void StackDump_(Stack *stack, FILE *file,
    const char *stack_variable_name,
    const char *stack_function_name,
    const char *stack_file,
    const int   stack_line,
    const char *programm_function_name,
    const char *programm_file,
    const int   programm_line);


#endif