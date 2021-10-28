#include <stdio.h>
#include <Windows.h>

#include "StackLibrary\Stack.h"
#include "StackLibrary\StackProtection.h"

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    
    FILE* file = fopen("StackLibrary.html", "w");
    stackLogFile = file;
    StackLogConstructor(stackLogFile);
    
    Stack stk = {};

    StackConstructor(&stk, sizeof(int), 0);

    int val = 10;
    for (int st=0; st<1000;st++)
        StackPush(&stk, &val);
    
    for (int st=0; st<1000;st++)
        StackPop(&stk, nullptr);

    //StackDump(&stk, stackLogFile);

    StackDestructor(&stk);
    fclose(stackLogFile);

    return 0;
}
