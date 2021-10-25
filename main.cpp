#include <stdio.h>
#include <Windows.h>

#include "StackLibrary\Stack.h"


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    
    FILE* file = fopen("StackLibrary.log", "w");
    stackLogFile = file;
    
    Stack stk = {};

    StackConstructor(&stk, sizeof(int), 0);

    int val = 10;
    StackPop(&stk, nullptr);

    StackDump(&stk, stackLogFile);

    StackDestructor(&stk);
    fclose(stackLogFile);

    return 0;
}
