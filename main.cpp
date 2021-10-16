#include <stdio.h>
#include <Windows.h>

#include "StackLibrary\Stack.h"

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Stack stack = {};

    Stack_Constructor(&stack, sizeof(int), 16);

    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    for (size_t st = 0; st < sizeof(values)/sizeof(values[0]); st++)
    {
        StackPush(&stack, values + st);
        getchar();
    }

    Stack_Destructor(&stack);

    return 0;
}