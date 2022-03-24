/**
 *       Author: Kidd Chen
 *       Date  : 2022/02/20
 *       Description: Statistics on how many code lines
 *       1. Includes file types, .h, .c, .cpp, .cs
 *       2. Make sure these file are in utf-8 encoding, here provides python script <cover_2_uft8.py> to help convert
 *          them. The usage please refer to attached picture.
 *       3. Besides our FPC internal files, also support any other files. Just choice it per prompt in command windows.
 *       4. Only statistics for effective codes, exclude those comment codes.
 */

#include "config.h"
#include "tools.h"
#include "parser_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

FILE* ChoiceFileToStatistics()
{
    FILE *fr;
    int choice = 0;

    printf("Please choise what files you want to statistics(1 or 2): \n1. All code files \n2. Only FPC internal files\n");
    while (true)
    {
        scanf_s("%d", &choice);
        if (choice == 1)
        {
            fr = OpenFile(ALL_FILES_LOG, "r");
            break;
        }
        else if (choice == 2)
        {
            fr = OpenFile(FPC_FILES_LOG, "r");
            break;
        }
        else
        {
            printf("Input a wrong choice, please choice again!\n");
        }
    }
    return fr;
}

void GetAllFilesPath()
{
    char command[300] = "DIR /D /S /B ";
    printf("Please input src code path: \n");
    char path[MAX_PATH_LEN] = { 0 };
    scanf("%s", path);      // sample: E:\workspace\mtt
    strcat(command, path); strcat(command, " > "); strcat(command, OUTPUT_ALL_FILES_LIST);
    system(command);

    GetAllFilesNamePath();
    GetFPCFilesNamePath();
}

void GetAllFilesNamePath()
{
    int path_len = 0, count = 0;
    char filename[MAX_PATH_LEN] = { 0 };
    char *allfilepath[5000] = { 0 };
    FILE *fr;

    fr = OpenFile(OUTPUT_ALL_FILES_LIST, "r");
    while (fgets(filename, MAX_PATH_LEN, fr) != NULL)
    {
        TrimString(filename);
        path_len = strlen(filename);

        if (IsCFile(filename) || IsCSFile(filename))
        {
            char *temp = (char*)calloc(path_len, sizeof(char));
            memcpy(temp, filename, path_len);
            temp[path_len] = '\0';
            allfilepath[count++] = temp;
            temp = NULL;
            free(temp);
        }
    }
    fclose(fr);

    WriteFileNameToLog(allfilepath, count, 1);
}

void GetFPCFilesNamePath()
{
    int path_len = 0, count = 0;
    char filename[MAX_PATH_LEN] = { 0 }, line[MAX_PATH_LEN] = { 0 }, *str = NULL;
    char *allfpcfile[5000] = { 0 };
    FILE *fr;

    fr = OpenFile(OUTPUT_ALL_FILES_LIST, "r");
    while (fgets(filename, MAX_PATH_LEN, fr) != NULL)
    {
        TrimString(filename);
        path_len = strlen(filename);

        if (IsCFile(filename) || IsCSFile(filename))
        {
            if (IsFPCFile(filename))
            {
                char *temp = (char*)calloc(path_len, sizeof(char));
                memcpy(temp, filename, path_len);
                temp[path_len] = '\0';
                allfpcfile[count++] = temp;
                temp = NULL;
                free(temp);
            }
        }
    }
    fclose(fr);

    WriteFileNameToLog(allfpcfile, count, 2);
}

int AllFpcFilesCount(FILE *fp)
{
    if (fp == NULL)
    {
        printf("%p is NULL", fp);
        exit(-1);
    }

    int count = 0;
    char line[MAX_PATH_LEN] = { 0 };

    fp = OpenFile(FPC_FILES_LOG, "r");
    while (fgets(line, MAX_PATH_LEN, fp) != NULL)
    {
        count++;
    }
    fclose(fp);

    return count;
}
