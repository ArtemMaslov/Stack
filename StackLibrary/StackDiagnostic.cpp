#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Stack.h"
#include "StackDiagnostic.h"
#include "StackProtection.h"


int ValidateStack(Stack *stack)
{
    int error = STACKERR_NO_ERRORS;
    if (stack == nullptr)
    {
        error |= STACKERR_STACK_IS_NULL;
        //assert("Stack ERROR" == "Stack pointer is null");
    }
    else
    {
        if (stack->canary1 != STACK_CANARY1)
            error |= STACKERR_CANARY1;

        if (stack->canary1 != STACK_CANARY2)
            error |= STACKERR_CANARY2;
        
        if (stack->data == nullptr || stack->data == STACK_NULL_PTR && stack->stackCapacity>0)
            error |= STACKERR_STACK_IS_EMPTY;
        
        if (!CheckStackCRC(stack))
            error |= STACKERR_STACK_CRC;

        if ((error & STACKERR_STACK_IS_EMPTY) == 0)
        {
            if (!CheckDataCRC(stack))
                error |= STACKERR_DATA_CRC;
        }

        if (stack->stackSize < stack->stackCapacity)
            error |= STACKERR_SIZE_MORE_CAPACITY;
    }
    return error;
}

bool IsStackEmpty(Stack *stack)
{
    if (stack->data          == nullptr &&
        stack->stackSize     == 0 &&
        stack->stackCRC      == 0 &&
        stack->dataCRC       == 0 &&
        stack->canary1       == 0 &&
        stack->canary2       == 0 &&
        stack->elementSize   == 0 &&
        stack->stackCapacity == 0)
        return true;
    else
        return false;
}

void ClearStack(Stack *stack)
{
    stack->elementSize   = 0;
    stack->stackCapacity = 0;
    stack->stackSize     = 0;
    stack->data          = nullptr;
    stack->stackSize     = 0;
    stack->stackCRC      = 0;
    stack->dataCRC       = 0;
    stack->canary1       = 0;
    stack->canary2       = 0;
    stack->elementSize   = 0;
    stack->stackCapacity = 0;
}

bool CheckForError(int errors, StackErrors error)
{
    if ((errors & error) > 0)
        return false;
    else
        return true;
}

void StackDump_(Stack *stack, FILE *file,
    const char *stack_variable_name,
    const char *stack_function_name,
    const char *stack_file,
    const int   stack_line,
    const char *programm_function_name,
    const char *programm_file,
    const int   programm_line)
{

    char stackState[4] = "ERR";
    char buffer[100] = "";

    unsigned int stackError = ValidateStack(stack);

    if (stack != nullptr && stackError == STACKERR_NO_ERRORS)
        strcpy_s(stackState, "OK");

    sprintf_s(buffer, "Stack<%s> [0x%p]: %s \"%s\" called ", 
        StackTypeName,
        stack,
        stackState,
        stack_variable_name);

    fputs(buffer, file);

    fprintf(file, "from % s() at |%s, %d| \n", stack_function_name, stack_file, stack_line);

    fprintf(file, "%*sfrom %s() at |%s, %d|\n", strlen(buffer), "", programm_function_name, programm_file, programm_line);

    if (stackError > 0)
    {
        fprintf(file, "Erros = %d:\n", stackError);

        int arrayIndex = 1;
        while (stackError > 0)
        {
            if (stackError % 2 > 0)
                fprintf(file, "    * %s\n", StackErrorStrings[arrayIndex]);
            arrayIndex++;
            stackError /= 2;
        }
    }
    fputs("Structure:\n{\n", file);

    if (stack)
    {
        fprintf(file, "%*s canary1       = %ld\n", 3, "", stack->canary1);

        fprintf(file, "%*s elementSize   = %zd\n", 3, "", stack->elementSize);
        fprintf(file, "%*s stackSize     = %zd\n", 3, "", stack->stackSize);
        fprintf(file, "%*s stackCapacity = %zd\n", 3, "", stack->stackCapacity);
        
        fprintf(file, "%*s dataCRC       = %ld\n", 3, "", stack->dataCRC);
        fprintf(file, "%*s stackCRC      = %ld\n", 3, "", stack->stackCRC);
        
        fprintf(file, "%*s canary2       = %ld\n", 3, "", stack->canary2);

        fprintf(file, "\n    data [0x%p]:\n    {\n", stack->data);

        if (stack->data)
        {
            size_t capacity = stack->stackCapacity;
            size_t size     = stack->stackSize;
            int    *data     = (int*)stack->data;

            size_t numberLength = 0;
            while (capacity > 0)
            {
                capacity /= 10;
                numberLength++;
            }
            capacity = stack->stackCapacity;

            for (size_t st = 0; st < capacity; st++)
            {
                fprintf(file, "%*s %c[%*.1d] = %d\n", 7, "", (st <= size) ? '*' : ' ', numberLength, st, data[st]);
            }
        }

        fputs("    }\n", file);
    }

    fputs("}", file);
}