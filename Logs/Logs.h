#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>

/**
 * @brief      Констркуктор файла логов.
 * @param file Указатель на поток вывода.
*/
void LogConstructor(FILE *file);

/**
 * @brief      Закрывает файл логов.
 * @param file Указатель на поток вывода.
*/
void LogDestructor(FILE* file);

/**
 * @brief         Добавляет строку в файл логов.
 * @param file    Указатель на поток вывода.
 * @param message Строка, которую необходимо добавить.
*/
void LogLine(FILE* file, const char* message);


#endif // !LOGS_H_
