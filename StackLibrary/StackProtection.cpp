#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "Stack.h"
#include "StackProtection.h"


bool CheckDataCRC(Stack *stack)
{
    if (stack)
    {
        if (stack->data && stack->dataCRC != CalculateCRC((char*)stack->data, stack->stackCapacity))
            return false;
        else
            return true;
    }
    return false;
}

bool CheckStackCRC(Stack *stack)
{
    if (stack)
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
    return false;
}

void CalculateDataCRC(Stack *stack)
{
    if (stack->data)
        stack->dataCRC = CalculateCRC((char*)stack->data, stack->stackCapacity);
}

void CalculateStackCRC(Stack *stack)
{
    if (stack)
    {
        CalculateDataCRC(stack);
        stack->stackCRC = 0;
        stack->stackCRC = CalculateCRC((char*)(stack), sizeof(Stack));
    }
}

static int64_t CalculateCRC(char *buf, size_t len)
{
    assert(buf);

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

