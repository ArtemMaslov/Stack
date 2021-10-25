#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>

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
void LogLine(FILE* file, const char* message);


#endif // !LOGS_H_
