#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Stack.h"
#include "StackDiagnostic.h"
#include "StackProtection.h"

#include "..\Logs\Logs.h"

extern FILE* stackLogFile;

int ValidateStack(Stack *stack)
{
#ifdef StackLogs
    LogLine(stackLogFile, "ValidateStack");
#endif

    int error = STACKERR_NO_ERRORS;
    if (stack == nullptr)
    {
        error |= STACKERR_PTR_IS_NULL;
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack ptr is null");
#endif
    }
    else
    {
        if (stack->canary1 != STACK_CANARY1)
        {
            error |= STACKERR_CANARY1;
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack canary1 warning");
#endif
        }

        if (stack->canary2 != STACK_CANARY2)
        {
            error |= STACKERR_CANARY2;
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack canary2 warning");
#endif
        }
        
        if (stack->data == nullptr && stack->stackCapacity>0)
        {
            error |= STACKERR_STACK_IS_EMPTY;
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack stack is empty");
#endif
        }
        
        if (!CheckStackCRC(stack))
        {
            error |= STACKERR_STACK_CRC;
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack stack structure crc");
#endif
        }

        if ((error & STACKERR_STACK_IS_EMPTY) == 0 && stack->data)
        {
            if (!CheckDataCRC(stack))
            {
                error |= STACKERR_DATA_CRC;
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack stack data crc");
#endif
            }
            
            if (((int64_t*)((char*)stack->data - sizeof(int64_t)))[0] != STACK_CANARY1)
            {
                error |= STACKERR_DATA_CANARY1;
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack data canary1");
#endif
            }
            if (((int64_t*)((char*)stack->data + stack->stackCapacity * stack->elementSize))[0] != STACK_CANARY2)
            {
                error |= STACKERR_DATA_CANARY2;
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack data canary1");
#endif
            }
        }

        if (stack->stackSize >= stack->stackCapacity && stack->stackCapacity != 0)
        {
            error |= STACKERR_SIZE_MORE_CAPACITY;
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack stack size more capacity");
#endif
        }
    }
    return error;
}

int IsStackEmpty(Stack *stack)
{
    if (stack->data          == nullptr &&
        stack->stackSize     == 0 &&
        stack->stackCRC      == 0 &&
        stack->dataCRC       == 0 &&
        stack->canary1       == 0 &&
        stack->canary2       == 0 &&
        stack->elementSize   == 0 &&
        stack->stackCapacity == 0)
        return STACKERR_NO_ERRORS;
    else
        return STACKERR_STACK_IS_NOT_INITED;
}

void ClearStack(Stack *stack)
{
#ifdef StackLogs
    LogLine(stackLogFile, "ClearStack");
#endif
    memset(stack, 0, sizeof(Stack));
}

bool CheckForError(int errors, StackErrors errorName)
{
    if ((errors & errorName) > 0)
        return true;
    else
        return false;
}

bool IsStackBroken(int errors, Stack* stack)
{
    if (stack)
    {
        if (CheckForError(errors, STACKERR_PTR_IS_NULL) ||
            CheckForError(errors, STACKERR_ELEM_SIZE_INVALIDE) ||
            CheckForError(errors, STACKERR_NO_MEMORY) ||
            CheckForError(errors, STACKERR_STACK_IS_NOT_INITED))
            return true;
        return false;
    }
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
#ifdef StackLogs
    LogLine(stackLogFile, "StackDump_");
#endif

    char stackState[4] = "ERR";
    char buffer[100] = "";

    unsigned int stackError = ValidateStack(stack);

    if (stack != nullptr && stackError == STACKERR_NO_ERRORS)
        strcpy_s(stackState, "OK");

    sprintf_s(buffer, "\nStackDump\nStack<%s> [0x%p]: %s \"%s\" called ", 
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