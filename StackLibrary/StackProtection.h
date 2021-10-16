#ifndef STACK_PROTECTION_H_
#define STACK_PROTECTION_H_


#include "Stack.h"

bool CheckDataCRC(Stack *stack);

bool CheckStackCRC(Stack *stack);

int64_t CalculateCRC(char *buf, size_t len);


#endif