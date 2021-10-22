#include <stdio.h>
#include <Windows.h>

#include "StackLibrary\Stack.h"


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    
    Stack stk = {};
    StackConstructor(&stk, sizeof(int), 0);

    for (int st = 0; st < 100; st++)
    {
        int value = st + 1;
        StackPush(&stk, &value);
    }

    StackDump(&stk, stdout);

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
    
    StackDump(&stk, stdout);
    return 0;
}
