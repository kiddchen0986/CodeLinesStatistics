/**
 *       Author: Kidd Chen
 *       Date  : 2022/02/20
 *       Description: Statistics on how many code lines
 *       1. Includes file types, .h, .c, .cpp, .cs
 *       2. available to choise what kinds of files, means count all codes or just written by ourselves
 *       3. Only for effective codes, exclude those comment codes
 *       4. Important to make sure all files are in utf-8 encoding, thus so you please first
 *          run the python script <cover_2_uft8.py> here provided to convert them.
 */

#include "stdafx.h"
#include "define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "conio.h"

#define MAX_PATH_LEN 260

#define OUTPUT_ALL_FILES_LIST "List_All_Files_Path"  // all files and folders
#define ALL_FILES_LOG "All_Files_Path"               // all files to count
#define FPC_FILES_LOG "FPC_Files_Path"               // only fpc internal files to count

void GetAllFilesPath();
void GetAllFilesNamePath();
void GetFPCFilesNamePath();
FILE *ChoiceFileToStatistics();
FILE *OpenFile(char *file, char *mode);
char *TrimString(char *line);
bool IsCFile(char *filename);
bool IsCSFile(char *filename);
bool IsFPCFile(char *filename);
int AllFpcFilesCount(FILE *fp);
void IgnoreCommentType1(FILE *fp, char *str);  // i.e. /* ... */, include single line and multi lines
void WriteFileNameToLog(char *fpcfilepath[], int count, int type);

int main()
{
    int c_file_number = 0, cs_file_number = 0;
    int sum = 0, c_sum = 0, cs_sum = 0, count = 0;
    char filename[MAX_PATH_LEN] = { 0 }, str[MAX_PATH_LEN] = { 0 }, filechoice[30] = { 0 }, *line = NULL;
    FILE *fr, *fw, *fp;

    GetAllFilesPath();
    fw = OpenFile("log", "w");

    fr = ChoiceFileToStatistics();
    while (fgets(filename, MAX_PATH_LEN, fr) != NULL)
    {
        printf("File name: %s\n", TrimString(filename));
        fprintf_s(fw, "File name: %s\n", TrimString(filename));

        count = 0;
        fp = OpenFile(filename, "r");
        while (fgets(str, MAX_PATH_LEN, fp) != NULL)
        {
            line = TrimString(str);
            if ((line[0] == '/') && (line[1] == '*'))
            {
                // ignore comment type 1, i.e. /* ... */
                IgnoreCommentType1(fp, line);
                continue;
            }
            else if ((line[0] == '/') && (line[1] == '/'))
            {
                // ignore comment type 2, i.e. //
                continue;
            }
            else if (strlen(line) == 0)
            {
                // ignore blank line
                continue;
            }
            count++;
        }
        fclose(fp);

        if (IsCFile(filename))
        {
            ++c_file_number;
            c_sum += count;
        }
        else if(IsCSFile(filename))
        {
            ++cs_file_number;
            cs_sum += count;
        }
        sum += count;
        printf("Its number of lines: %d\n\n", count);
        fprintf(fw, "Its number of lines: %d\n\n", count);
    }

    printf("\n********************************\n");
    printf("Total number of files : %d, total line number : %d\n", (c_file_number + cs_file_number), sum);
    printf("The number of c files : %d, line number : %d\n", c_file_number, c_sum);
    printf("The number of cs files : %d, line number : %d\n", cs_file_number, cs_sum);
    printf("********************************\n");
    fprintf(fw, "Total line number of all files is : %d\n", sum);
    fprintf(fw, "The number of c files : %d, line number : %d\n", c_file_number, c_sum);
    fprintf(fw, "The number of cs files : %d, line number : %d\n", cs_file_number, cs_sum);

    fclose(fr);
    fclose(fw);

    return 0;
}

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
    strcat_s(command, path); strcat_s(command, " > "); strcat_s(command, OUTPUT_ALL_FILES_LIST);
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

void WriteFileNameToLog(char *fpcfilepath[], int count, int type)
{
    FILE *fw;

    fw = type == 1 ? OpenFile(ALL_FILES_LOG, "w") : OpenFile(FPC_FILES_LOG, "w");
    for (int i = 0; i < count; i++)
    {
        fputs(fpcfilepath[i], fw);
        fputc('\n', fw);
    }
    fclose(fw);
}

FILE *OpenFile(char *file, char *mode)
{
    FILE *fp;
    if ((fp = fopen(file, mode)) == NULL)
    {
        printf("%p can not be opened", file);
        exit(0);
    }

    return fp;
}

char *TrimString(char *line)
{
    if (strlen(line) == 0)
    {
        return line;
    }

    // left trim
    for (int i = 0; i < MAX_PATH_LEN && line[i] != '\0'; i++)
    {
        if (isspace(line[i--]))
        {
            line = line + 1;
        }
        else
        {
            break;
        }
    }

    //right trim
    for (int i = strlen(line); i >= 0; i--)
    {
        if (line[i - 1] > 127 || line[i - 1] < 0)              // remove non - ascii character
        {
            line[i - 1] = '\0';
        }
        if ((line[i - 1] == '\n') || (isspace(line[i - 1])))   // remove '\n' and space
        {
            line[i - 1] = '\0';
        }
        else
        {
            break;
        }
    }

    return line;
}

bool IsCFile(char *filename)
{
    if (filename == NULL)
    {
        printf("%s is NULL string!\n", filename);
        exit(1);
    }

    bool status = false;
    int len = strlen(filename);
    if ((filename[len - 2] == '.') && ((filename[len - 1]) == 'h'))
    {
        status = true;
    }
    else if (((filename[len - 2] == '.') && ((filename[len - 1]) == 'c')))
    {
        status = true;
    }
    else if (filename[len - 1] == 'p' && filename[len - 2] == 'p' &&
        filename[len - 3] == 'c' && filename[len - 4] == '.')
    {
        status = true;
    }

    return status;
}

bool IsCSFile(char *filename)
{
    if (filename == NULL)
    {
        printf("%s is NULL string!\n", filename);
        exit(1);
    }

    bool status = false;
    int len = strlen(filename);
    if (filename[len - 3] == '.' && filename[len - 2] == 'c' && filename[len - 1] == 's')
    {
        status = true;
    }

    return status;
}

bool IsFPCFile(char *filename)
{
    FILE *fp = NULL;
    fp = OpenFile(filename, "r");

    int i = 0;
    bool status = false;
    char str[MAX_PATH_LEN] = { 0 };
    while ((fgets(str, MAX_PATH_LEN, fp) != NULL) && (i < 10))
    {
        TrimString(str);
        if ((strstr(str, "Fingerprint Cards") != NULL) &&
            (strstr(str, "fingerprints.com") != NULL))
        {
            status = true;
            break;
        }
        i++;
    }
    fclose(fp);
    return status;
}

void IgnoreCommentType1(FILE *fp, char *line)
{
    if ((fp == NULL) || (line == NULL))
    {
        printf("%p or %p is NULL string!\n", fp, line);
        exit(-1);
    }

    char str[MAX_PATH_LEN] = { 0 };
    if ((line[strlen(line) - 1] == '/') && (line[strlen(line) - 2] == '*'))   // end in the same line
    {
        ;
    }
    else    // end in new line
    {
        while (fgets(str, MAX_PATH_LEN, fp) != NULL)
        {
            line = TrimString(str);
            if (((line[0] == '*') && (line[1] == '/')) ||                               // at line's beginning
                ((line[strlen(line) - 1] == '/') && (line[strlen(line) - 2] == '*')))   // at line's ends
            {
                break;
            }
        }
    }
}
