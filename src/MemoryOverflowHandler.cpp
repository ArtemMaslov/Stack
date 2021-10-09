#include <stdlib.h>

static const int ptrArraySize = 100;
static const int ArraySize = 500;

static int *ptrArray[ptrArraySize];
static int ptrIndex = 0;


void InitMemoryHandlers()
{
    for (int st = 0; st < ptrArraySize; st++)
        ptrArray[st] = nullptr;

}

void *MallocH(size_t elementsCount, size_t elementSize)
{
    return malloc(elementsCount * elementSize);
}

void* CallocH(size_t elementsCount, size_t elementSize)
{
    return calloc(elementsCount, elementSize);
}

void ReallocH()
{

}