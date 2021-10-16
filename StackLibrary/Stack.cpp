#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Stack.h"
#include "StackProtection.h"
#include "StackDiagnostic.h"


int Stack_Constructor(Stack *stack, size_t elementSize, int Capacity = 16)
{
    if (stack == nullptr)
        return STACKERR_STACK_IS_NULL;

    if (IsStackEmpty(stack))
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

    ClearStack(stack);

    stack->data = STACK_NULL_PTR;

    return STACKERR_NO_ERRORS;
}

int StackPush(Stack *stack, void *value)
{
    int error = ValidateStack(stack);

    if (CheckForError(stack, STACKERR_SIZE_MORE_CAPACITY))
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
    
    memcpy((char*)stack->data + stack->elementSize * stack->stackSize++, value, stack->elementSize);

    error |= ValidateStack(stack);

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
    
    if (error)
        *error |= ValidateStack(stack);

    return (char*)stack->data + (stack->elementSize * (stack->elementSize + 1));
}

Stack* StackResize(Stack* stack, int direction, int* error)
{
    bool shouldResize = false;

    switch (direction)
    {
        case 1:
            if (stack->stackSize >= stack->stackCapacity)
            {
                stack->stackCapacity *= STACK_CAPACITY_SCALE;
                shouldResize = true;
            }
            break;
        case -1:
            if (stack->stackSize < stack->stackCapacity / STACK_CAPACITY_SCALE - STACK_CAPACITY_DECREASE_DELTA)
            {
                stack->stackCapacity = stack->stackCapacity / STACK_CAPACITY_SCALE - STACK_CAPACITY_DECREASE_DELTA;
                shouldResize = true;
            }
            break;
        default:
            if (error)
                *error = STACKERR_UNKNOWN_RESIZE_DIRECTION;
            return nullptr;
    }

    if (shouldResize)
    {
        stack = (Stack*)realloc(stack->data, stack->stackCapacity);

        if (stack == nullptr)
        {
            if (error)
                *error |= STACKERR_NO_MEMORY;

            StackDump(stack, stdout, "", "", -1);

            assert("StackPush Realloc returned value" == nullptr);
            return nullptr;
        }
    }

        return stack;
}