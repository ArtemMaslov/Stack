#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <cmath>

#include "..\Logs\Logs.h"
#include "Stack.h"
#include "StackProtection.h"
#include "StackDiagnostic.h"


static Stack* StackResize(Stack *stack, int *error);

#ifdef  StackLogs

FILE* stackLogFile = nullptr;

void InitStackLogFile(FILE *file)
{
    if (stackLogFile == nullptr)
    {
        LogConstructor("StackLogs.txt", file);
        stackLogFile = file;
    }
    //else
    //    assert(!"Stack log file already inited");
}

void CloseLogFile()
{
    if (stackLogFile)
        LogDestructor(stackLogFile);
}

#endif //  StackLogs


int StackConstructor(Stack *stack, size_t elementSize, size_t Capacity = 0)
{
#ifdef StackLogs
    LogLine(stackLogFile, "StackConstructor");
#endif

    if (stack == nullptr)
    {
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack ptr is null");
#endif
        return STACKERR_PTR_IS_NULL;
    }

    int error = 0;
    error |= IsStackEmpty(stack);
    error |= (elementSize > 0) ? STACKERR_NO_ERRORS : STACKERR_ELEM_SIZE_INVALIDE;

    if (error == STACKERR_NO_ERRORS)
    {
        stack->elementSize   = elementSize;
        stack->stackCapacity = Capacity;
        stack->stackSize     = 0;

        stack->canary1 = STACK_CANARY1;
        stack->canary2 = STACK_CANARY2;
        
        if (stack->stackCapacity > 0)
        {
            stack->stackCapacity = (stack->stackCapacity < STACK_MIN_CAPACITY) ? STACK_MIN_CAPACITY : stack->stackCapacity;
            stack->data = calloc(Capacity, stack->elementSize * Capacity);
        }

        CalculateStackCRC(stack);
    }

    return error;
}

int StackDestructor(Stack *stack)
{
#ifdef StackLogs
    LogLine(stackLogFile, "StackDestructor");
#endif

    if (stack == nullptr)
    {
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack ptr is null");
#endif
        return STACKERR_PTR_IS_NULL;
    }

    free((char*)stack->data - sizeof(int64_t));

    ClearStack(stack);

    return STACKERR_NO_ERRORS;
}

int StackPush(Stack *stack, void *value)
{
#ifdef StackLogs
    LogLine(stackLogFile, "StackPush");
#endif

    int error = ValidateStack(stack);

    if (IsStackBroken(error, stack))
        return error;

    stack = StackResize(stack, &error);

    if (stack)
    {
        memmove((char*)stack->data + stack->elementSize * stack->stackSize++, value, stack->elementSize);
        CalculateStackCRC(stack);
    }

    error |= ValidateStack(stack);

    return error;
}

void* StackPop(Stack *stack, int *error = nullptr)
{
#ifdef StackLogs
    LogLine(stackLogFile, "StackPop");
#endif

    int _error = ValidateStack(stack);

    if (IsStackBroken(_error, stack))
    {
        if (error)
            *error = _error;
        return nullptr;
    }

    if (stack->stackSize == 0)
    {
#ifdef StackLogErrors
        LogLine(stackLogFile, "Stack is empty");
#endif
        if (error)
            *error |= STACKERR_STACK_IS_EMPTY;
        return nullptr;
    }
    
    stack->stackSize--;

    stack = StackResize(stack, error);

    CalculateStackCRC(stack);
    
    _error |= ValidateStack(stack);
    if (error)
        *error |= _error;

    return (char*)stack->data + (stack->elementSize * stack->stackSize);
}

static Stack* StackResize(Stack *stack, int *error)
{
#ifdef StackLogs
    LogLine(stackLogFile, "StackResize");
#endif

    int oldCapacity = stack->stackCapacity;
    bool shouldResize = false;

    if (stack->stackCapacity == 0)
    {
        stack->stackCapacity = STACK_MIN_CAPACITY;
        shouldResize = true;
    }
    //else if (stack->stackSize == 0)
    //{
    //    stack->stackCapacity = 0;
    //    shouldResize = true;
    //}
    else if (stack->stackSize >= stack->stackCapacity)
    {
        stack->stackCapacity *= STACK_CAPACITY_SCALE;
        shouldResize = true;
    }
    else
    {
        int rawCapacity = (stack->stackCapacity / STACK_CAPACITY_SCALE - STACK_CAPACITY_DECREASE_DELTA);
        size_t newCapacity = rawCapacity < 0 ? 0 : rawCapacity;
        if (stack->stackSize < newCapacity)
        {
            stack->stackCapacity = newCapacity < STACK_MIN_CAPACITY ? STACK_MIN_CAPACITY : newCapacity;
            shouldResize = true;
        }
    }
        
    if (shouldResize)
    {
        char *dataPtr = nullptr;
        
        if (stack->data)
            dataPtr = (char*)stack->data - sizeof(int64_t);

        void *reallocResult = 
            realloc(dataPtr, sizeof(int64_t) + stack->stackCapacity * stack->elementSize + sizeof(int64_t));

        if (reallocResult == nullptr)
        {
    #ifdef StackLogErrors
        LogLine(stackLogFile, "Stack no memory");
    #endif
            if (error)
                *error |= STACKERR_NO_MEMORY;
            return nullptr;
        }

        stack->data = (char*)reallocResult + sizeof(int64_t);
        if (stack->stackCapacity > oldCapacity)
            memset((char*)stack->data + oldCapacity * stack->elementSize, 0, (stack->stackCapacity - oldCapacity) * stack->elementSize);

        ((int64_t*)( (char*)stack->data - sizeof(int64_t) ))[0] = STACK_CANARY1;

        ((int64_t*)( (char*)stack->data + stack->stackCapacity * stack->elementSize ))[0] = STACK_CANARY2;
    }

    return stack;
}
