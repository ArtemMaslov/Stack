#ifndef STACK_H_
#define STACK_H_


#include <stdint.h>

const char *StackTypeName = "unknown_type";

struct Stack
{
    int64_t canary1;
    size_t  elementSize;
    size_t  stackSize;
    size_t  stackCapacity;
    void*   data;
    int64_t dataCRC;
    int64_t stackCRC;
    int64_t canary2;
};

const char StackErrorStrings[][24] =
{
    "No errors",
    "Stack is null",
    "Stack is empty",
    "Stack is not empty",
    "No memmory",
    "Size more capacity",
    "Left canary is broken",
    "Right canary is broken",
    "Data CRC invalide",
    "Stack CRC invalide",
    "Stack isn't inited",
};

enum StackErrors
{
    STACKERR_NO_ERRORS           = 0 << 0,
    STACKERR_STACK_IS_NULL       = 1 << 0,
    STACKERR_STACK_IS_EMPTY      = 1 << 1,
    STACKERR_STACK_IS_NOT_EMPTY  = 1 << 2,
    STACKERR_NO_MEMORY           = 1 << 3,
    STACKERR_SIZE_MORE_CAPACITY  = 1 << 4,
    STACKERR_CANARY1             = 1 << 5,
    STACKERR_CANARY2             = 1 << 6,
    STACKERR_DATA_CRC            = 1 << 7,
    STACKERR_STACK_CRC           = 1 << 8,
    STACKERR_STACK_IS_NOT_INITED = 1 << 9,
};

#define STACK_NULL_PTR (void*)0x18
#define STACK_CAPACITY_SCALE 2
#define STACK_CAPACITY_DECREASE_DELTA 5

#define STACK_CANARY1 0xBADBADBAADF0000D
#define STACK_CANARY2 0xBADF0000DBAADBAD

int Stack_Constructor(Stack *stack, int elementSize, int Capacity);

int Stack_Destructor(Stack *stack);

int StackPush(Stack *stack, void *value);

void* StackPop(Stack *stack, int *error);

int ValidateStack(Stack *stack);

void StackDump(Stack *stack, FILE *file,
    const char *stack_variable_name,
    const char *stack_function_name,
    const char *stack_file,
    const int   stack_line,
    const char *programm_function_name,
    const char *programm_file,
    const int   programm_line);

#endif