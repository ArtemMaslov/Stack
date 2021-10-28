#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>

extern int TextOffset;

enum LogLevel
{
    DEBUG = 0,
    WARNING = 1,
    ERROR = 2
};

/**
 * @brief      ������������ ����� �����.
 * @param file ��������� �� ����� ������.
*/
void LogConstructor(FILE *file);

/**
 * @brief      ��������� ���� �����.
 * @param file ��������� �� ����� ������.
*/
void LogDestructor(FILE* file);

/**
 * @brief         ��������� ������ � ���� �����.
 * @param file    ��������� �� ����� ������.
 * @param message ������, ������� ���������� ��������.
*/
void LogLine(FILE* file, const char* message, int error = DEBUG);


#endif // !LOGS_H_
