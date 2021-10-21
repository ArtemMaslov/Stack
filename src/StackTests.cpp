#include <stdio.h>
#include "..\StackLibrary\Stack.h"


void StackTest3()
{
}

void StackTest2()
{
}

void StackTest1()
{
    Stack stk = {};
    StackConstructor(&stk, sizeof(int), 0);

    for (int st = 0; st < 100; st++)
    {
        int value = st + 1;
        StackPush(&stk, &value);
        printf("%d\n", ((int*)stk.data)[st]);
    }

    for (int st = 0; st < 100; st++)
    {
        if (((int*)stk.data)[st] != st + 1)
        {
            puts("Err");
        }
    }
    
    for (int st = 100; st > 0; st--)
    {
        int err = 0;
        int value = *((int*)StackPop(&stk, &err));
        if (value != st)
        {
            puts("Err");
        }
    }
}