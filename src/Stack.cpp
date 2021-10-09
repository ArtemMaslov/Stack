#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "..\inc\Stack.h"


bool CheckDataCRC(Stack *stack);

bool CheckStackCRC(Stack *stack);

int64_t CalculateCRC(char *buf, size_t len);



int Stack_Constructor(Stack *stack, int elementSize, int Capacity = 100)
{
    if (stack == nullptr)
        return STACKERR_STACK_IS_NULL;

    if (stack->data          == nullptr &&
        stack->stackSize     == 0 &&
        stack->stackCRC      == 0 &&
        stack->dataCRC       == 0 &&
        stack->canary1       == 0 &&
        stack->canary2       == 0 &&
        stack->elementSize   == 0 &&
        stack->stackCapacity == 0)
    {
        stack->elementSize   = elementSize;
        stack->stackCapacity = Capacity;
        stack->stackSize     = 0;

        stack->canary1 = STACKERR_CANARY1;
        stack->canary2 = STACKERR_CANARY2;

        
        stack->data = calloc(Capacity, stack->elementSize * Capacity);

        return STACKERR_NO_ERRORS;
    }
    else
        return STACKERR_STACK_IS_NOT_INITED;
}

int Stack_Destructor(Stack *stack)
{
    if (stack == nullptr)
        return STACKERR_STACK_IS_NULL;

    free(stack->data);

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

    stack->data = STACK_NULL_PTR;

    return STACKERR_NO_ERRORS;
}

int StackPush(Stack *stack, void *value)
{
    int error = ValidateStack(stack);

    if (stack->stackSize < stack->stackCapacity)
    {
        memcpy((char*)stack->data + stack->elementSize * stack->stackSize++, value, stack->elementSize);
    }
    else
    {
        size_t oldCapacity = stack->stackCapacity;

        // Переполнение size_t
        if (stack->stackCapacity * STACK_CAPACITY_SCALE < stack->stackCapacity)
        {
            assert (stack->stackCapacity * STACK_CAPACITY_SCALE >= stack->stackCapacity);
            error |= STACKERR_NO_MEMORY;
            return error;
        }

        stack->stackCapacity *= STACK_CAPACITY_SCALE;
        void *ptr = realloc(stack->data, stack->stackCapacity);

        if (ptr == nullptr)
        {
            assert("StackPush Realloc returned nullptr" == nullptr);
            error |= STACKERR_NO_MEMORY;
            return error;
        }

        stack->data = ptr;

        for (int st = oldCapacity; st < stack->stackCapacity; st++)
            st[(char*)ptr] = '\0';
    }

    return error;
}

void* StackPop(Stack *stack, int *error = nullptr)
{
    if (error)
        *error = ValidateStack(stack);

    if (stack->stackSize == 0)
    {
        if (error)
            *error |= STACKERR_STACK_IS_EMPTY;
        return nullptr;
    }
        
    stack->stackSize--;

    if ((stack->stackSize + STACK_CAPACITY_DECREASE_DELTA) * STACK_CAPACITY_SCALE < stack->stackCapacity)
    {
        stack->stackCapacity /= STACK_CAPACITY_SCALE;
        void *ptr = realloc(stack->data, stack->stackCapacity);

        if (ptr == nullptr)
        {
            assert("StackPush Realloc returned nullptr" == nullptr);
            if (error)
                *error |= STACKERR_NO_MEMORY;
            return nullptr;
        }
    }

    return (char*)stack->data + (stack->elementSize * (stack->elementSize + 1));
}

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

bool CheckDataCRC(Stack *stack)
{
    if (stack->dataCRC != CalculateCRC((char*)stack->data, stack->stackCapacity))
        return false;
    else
        return true;
}

bool CheckStackCRC(Stack *stack)
{
    int64_t stackCRC = stack->stackCRC;
    stack->stackCRC = 0;
    if (stackCRC != CalculateCRC((char*)(stack), sizeof(Stack)))
    {
        stack->stackCRC = stackCRC;
        return false;
    }
    else
    {
        stack->stackCRC = stackCRC;
        return true;
    }
}

int64_t CalculateCRC(char *buf, size_t len)
{
    uint64_t crc_table[256];
    uint64_t crc;

    for (int i = 0; i < 256; i++)
    {
        crc = i;
        for (int j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

        crc_table[i] = crc;
    };

    crc = 0xFFFFFFFFUL;

    while (len--) 
        crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

    return (int64_t)(crc ^ 0xFFFFFFFFUL);
};

void StackDump(Stack *stack, FILE *file,
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
        strcpy(stackState, "OK");

    sprintf(buffer, "Stack<%s> [%p]: %s \"%s\" called ", 
        StackTypeName,
        stack,
        stackState,
        stack_variable_name);

    fputs(buffer, file);

    fprintf(file, "from % s() at | % s, % d | \n", stack_function_name, stack_file, stack_line);


    fprintf(file, "%*sfrom %s() at |%s, %d|\n", strlen(buffer), programm_function_name, programm_file, programm_line);

    if (stackError > 0)
    {
        fprintf(file, "Erros = %d:\n", stackError);

        int arrayIndex = 1;
        while (stackError > 0)
        {
            if (stackError % 2 > 0)
                fprintf(file, "    * %s\n", StackErrorStrings[arrayIndex]);
            arrayIndex++;
        }
    }
    fputs("Structure:\n{\n", file);

    if (stack)
    {
        fprintf(file, "%*s canary1       = %ld\n", 3, stack->canary1);

        fprintf(file, "%*s elementSize   = %zd\n", 3, stack->elementSize);
        fprintf(file, "%*s stackSize     = %zd\n", 3, stack->stackSize);
        fprintf(file, "%*s stackCapacity = %zd\n", 3, stack->stackCapacity);
        
        fprintf(file, "%*s dataCRC       = %ld\n", 3, stack->dataCRC);
        fprintf(file, "%*s stackCRC      = %ld\n", 3, stack->stackCRC);
        
        fprintf(file, "%*s canary2       = %ld\n", 3, stack->canary2);

        fprintf(file, "data [%p]:\n{\n", stack->data);

        if (stack->data)
        {
            size_t capacity = stack->stackCapacity;
            size_t size     = stack->stackSize;
            int*   data     = (int*)stack->data;

            for (size_t st = 0; st < capacity; st++)
            {
                fprintf(file, "%*s %c[%.#d]", 7, (st <= size)?'*':' ', )
            }
        }1

        fputs("}\n", file);
    }

    fputs("}", file);

         data [0x0000143125]:
         {
              *[0] = 10
              *[1] = 13
              *[2] = 30
              [3] = -666
              [4] = -666
              [5] = -666
              [6] = -666
         }
}