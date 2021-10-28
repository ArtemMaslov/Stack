#ifndef STACK_H_
#define STACK_H_


#include <stdint.h>

#define StackLogs
#define StackLogErrors

#ifdef  StackLogs

extern FILE* stackLogFile;
#else
#define stackLogFile nullptr
#endif //  StackLogs

/**
 * @brief ��������� ������ ����.
*/
struct Stack
{
    int64_t canaryLeft;    /// ����� ���������.

    size_t  elementSize;   /// ������ ������ ��������.
    size_t  stackSize;     /// ������� ������ �����. ���������� ���������, ����������� � ����� � ������ ������.
    size_t  stackCapacity; /// ����������� �����, ������������ ����� ���������, ������� ����� ������� ����.

    void*   data;          /// ������ ������.

    int64_t dataCRC;       /// ����������� ����� ������� ������.
    int64_t stackCRC;      /// ����������� ����� ��������� ����.

    int64_t canaryRight;   /// ������ ���������.
};

/**
 * @brief ������ ����� ��� ������ ���������� �� ������� ����� � ����.
*/
const char StackErrorStrings[][40] =
{
    "No errors",
    "Stack is null",
    "Stack is empty",

    "No memory",
    "Size more capacity",
    "Left canary is broken",

    "Right canary is broken",
    "Data CRC invalide",
    "Stack CRC invalide",

    "Stack isn't inited",
    "Stack element size invalide",
    "Stack data left canary is broken",
    
    "Stack data right canary is broken",
    "Trying to push null value",
};

/**
 * @brief ������������ ������ �����. ������ ������ ������������ ����������� ��� �����. `0` ��������, ��� ������ ���.
*/
enum StackError
{
    STACKERR_NO_ERRORS           = 0 << 0,
    STACKERR_PTR_IS_NULL         = 1 << 0,
    STACKERR_STACK_IS_EMPTY      = 1 << 1,

    STACKERR_NO_MEMORY           = 1 << 2,
    STACKERR_SIZE_MORE_CAPACITY  = 1 << 3,
    STACKERR_LEFT_CANARY         = 1 << 4,

    STACKERR_RIGHT_CANARY        = 1 << 5,
    STACKERR_DATA_CRC            = 1 << 6,
    STACKERR_STACK_CRC           = 1 << 7,

    STACKERR_STACK_IS_NOT_INITED = 1 << 8,
    STACKERR_ELEM_SIZE_INVALIDE  = 1 << 9,
    STACKERR_DATA_LEFT_CANARY    = 1 << 10,

    STACKERR_DATA_RIGHT_CANARY   = 1 << 11,
    STACKERR_NULL_VALUE          = 1 << 12,
};

/// @brief ����������� ���������� ������ �����.
/// ���� ����� ��������� ������ ������ ���� ������� �����, ���� �� ������� STACK_MIN_CAPACITY.
#define STACK_MIN_CAPACITY 10
/// @brief ����������� ����������/������� ������� �����. ������ ���� ������ 1.
/// ��� ���������� ������������, ����������� ���������� ��������� ������.
/// ��� ���������� ������������, ���� ����� �������� ������ ������.
/// ����������� ����� ����� ������������� � STACK_CAPACITY_SCALE_COEFFICIENT ���, ���� stackSize >= stackCapacity.
/// ����������� ����� ����� �����������, ���� stackSize < newStackCapacity
/// newStackCapacity = min ( oldCapacity * (0.5 - STACK_CAPACITY_DECREASE_COEFFICIENT) ; abs(oldCapacity / 2.0 - STACK_MIN_CAPACITY) )
/// ��� ����, ���� � ����� ���� ��������, �� ����������� ����� �� ����� ����� ������ STACK_MIN_CAPACITY.
/// ���� stackSize == 0, �� ����������� ����� ��������������� ������ 0.
#define STACK_CAPACITY_SCALE_COEFFICIENT 2
/// @brief ����������� ���������� ����������� �����. ������ ���� �� ������ 0, �� ������ 0.5.
/// ��� ���������� ������������, ����������� ���������� ��������� ������.
/// ��� ���������� ������������, ���� ����� �������� ������ ������.
#define STACK_CAPACITY_DECREASE_COEFFICIENT 0.1
/// @brief �������� ����� ��������� ��� ����� � ������� ������.
#define STACK_LEFT_CANARY_VALUE 0xBAD0AAAAAAAA0BAD
/// @brief �������� ������ ��������� ��� ����� � ������� ������.
#define STACK_RIGHT_CANARY_VALUE 0xBADFBBBBBBBBFBAD

/**
 * @brief             ����������� �����.
 * @param stack       ��������� �� ����.
 * @param elementSize ������ ������ �������� � ������� data.
 * @param Capacity    ��������� ����������� �����. �� ��������� 0. ���� �������� ������ 0, �� ��� ������ ����� �������� ������.
 * @return            ��� ������, 0 - � ������ ��������� ����������.
*/
int   StackConstructor(Stack *stack, size_t elementSize, size_t Capacity = 0);

/**
 * @brief       ���������� �����.
 * @param stack ��������� �� ����.
 * @return      ��� ������, 0 - � ������ ��������� ����������.
*/
int   StackDestructor(Stack *stack);

/**
 * @brief       ��������� ������� � ����� �����.
 * @param stack ��������� �� ����.
 * @param value ��������� �� ��������, ������� ����� �������� � ����.
 * @return      ��� ������, 0 - � ������ ��������� ����������.
*/
int   StackPush(Stack *stack, void *value);

/**
 * @brief       ������� ������� �� ����� ����� � ���������� ���.
 * @param stack ��������� �� ����.
 * @param error ���� �� nullptr, �� ���� ���������� ����� ��������� ���� ������, ������� ��������� �� ����� ������ �������.
 * @return      �������� ��������� �� ����� ��������, nullptr - � ������ ������.
*/
void* StackPop(Stack *stack, int *error = nullptr);

/**
 * @brief       ��������� ���� �� ������� ������.
 * @param stack ��������� �� ����.
 * @return      ���������� ���� ������ �������� ��������� �����.
*/
int   ValidateStack(Stack *stack);

//#define StackDump(stack, file, programm_function_name, programm_file, programm_line) \
StackDump_(stack, file, #stack, __FUNCTION__, __FILE__, __LINE__, programm_function_name, programm_file, programm_line);

/**
 * @brief       ������� ��������� ���������� � ������� ��������� �����.
 * @param stack ��������� �� ����.
 * @param file  ��������� �� ����� ������
*/
#define StackDump(stack, file) \
StackDump_(stack, file, #stack, __FUNCTION__, __FILE__, __LINE__, "", "", -1);

/**
 * @brief                        ���������� �������. ����������� StackDump ������.
 * @param stack                  ��������� �� ����.
 * @param file                   ��������� �� ����� ������.
 * @param stack_variable_name    ��� ���������� � ���������� StackLibrary.
 * @param stack_function_name    ��� ������� � ���������� StackLibrary.
 * @param stack_file             ���� ���������� StackLibrary.
 * @param stack_line             ����� ������ ���� � ���������� StackLibrary.
 * @param programm_function_name ��� ������� � ���������, ������������ StackLibrary.
 * @param programm_file          ��� ����� � ���������, ������������ StackLibrary.
 * @param programm_line          ����� ������ ���� � ���������, ������������ StackLibrary.
*/
void StackDump_(Stack *stack, FILE *file,
    const char *stack_variable_name,
    const char *stack_function_name,
    const char *stack_file,
    const int   stack_line,
    const char *programm_function_name,
    const char *programm_file,
    const int   programm_line);

/**
 * @brief      ������������ ����� ����� �����.
 * @param file ��������� �� ����� ������.
*/
void StackLogConstructor(FILE* file);


#endif