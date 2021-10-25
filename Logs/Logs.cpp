#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <assert.h>

void LogConstructor(FILE *file)
{
    assert(file);

    time_t rawTime = time(nullptr);
    tm* curTime = localtime(&rawTime);

    const char* dateFormat = "00:00:00 01.01.2021";
    size_t dateLength = strlen(dateFormat) + 1;
    char* date = (char*)calloc(dateLength, sizeof(char));

    if (date)
    {
        strftime(date, dateLength, "%H:%M:%S %d.%m.%Y", curTime);

        fprintf(file, "��� ���� ��������� ����.\n"
                      "StackLibrary.\n"
                      "���� ��������: %s.\n\n", date);

        free(date);
    }
    else
    {
        fprintf(file, "��� ���� ��������� ����.\n"
                      "StackLibrary.\n"
                      "\n\n\n!!!!�������� ������!!!!\n\n\n", date);
    }
}

/**
 * @brief      ��������� ���� �����.
 * @param file ��������� �� ����� ������.
*/
void LogDestructor(FILE* file)
{
    if (file)
        fclose(file);
}

/**
 * @brief         ��������� ������ � ���� �����.
 * @param file    ��������� �� ����� ������.
 * @param message ������, ������� ���������� ��������.
*/
void LogLine(FILE* file, const char* message)
{
    assert(message);
    assert(file);

    time_t rawTime = time(nullptr);
    tm* curTime = localtime(&rawTime);

    const char* dateFormat = "00:00:00.000 > ";
    size_t dateLength = strlen(dateFormat) + 1;
    char* date = (char*)calloc(dateLength, sizeof(char));

    if (date)
    {
        strftime(date, dateLength, "%H:%M:%S. > ", curTime);
    
        fputs(date, file);
        free(date);
    }

    fputs(message, file);
    fputs("\n", file);

    fflush(file);
}