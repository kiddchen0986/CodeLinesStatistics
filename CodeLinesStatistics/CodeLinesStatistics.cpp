/**
 *       Author: Kidd Chen
 *       Date  : 2022/02/20
 *       Description: Statistics on how many code lines
 *       1. Includes file types, .h, .c, .cpp, .cs
 *       2. Just count codes written by ourselves
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

#define COMMAND "DIR /D E:\\workspace\\mtt\\32 /s /B > "
#define ALL_FILES_PATH_LOG "Files_Path.log"  // all files and folders
#define ALL_FILES_LOG "All_Files_path.log"   // all files to count
#define FPC_FILES_LOG "FPC_Files_Path.log"   // only fpc internal files to count

static int c_file_number = 0;
static int cpp_file_number = 0;
static int h_file_number = 0;
static int cs_file_number = 0;

void GetAllFilesPath();
void GetAllFilesNamePath();
void GetFPCFilesNamePath();
FILE *ChoiceFileToStatistics();
FILE *OpenFile(char *file, char *mode);
char *TrimString(char *line);
bool IsCFile(char *filename, int len);
bool IsCSFile(char *filename, int len);
bool IsFPCFile(FILE *fp, char *str);
int AllFpcFilesCount(FILE *fp);
void IgnoreCommentType1(FILE *fp, char *str);  // i.e. /* ... */, include single line and multi lines
void WriteFileNameToLog(char *fpcfilepath[], int count, int type);
int main()
{
    FILE *fl, *fp;
    int len = 0, sum = 0;
    long int count = 0;
    char filename[MAX_PATH_LEN] = { 0 }, str[MAX_PATH_LEN] = { 0 }, filechoice[30] = { 0 }, *line = NULL;

    GetAllFilesPath();
    fl = ChoiceFileToStatistics();

    while (fgets(filename, MAX_PATH_LEN, fl) != NULL)
    {
        printf("File name: %s\n", TrimString(filename));

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
        sum += count;
        printf("Its number of lines: %d\n\n", count);
    }

    printf("\n**********************\n");
    printf("Total line number of all files is : %d\n", sum);
    printf("The total number of h file: %d\n", h_file_number);
    printf("The total number of c file: %d\n", c_file_number);
    printf("The total number of cpp file: %d\n", cpp_file_number);
    printf("The total number of cs file: %d\n", cs_file_number);
    fclose(fl);

    return 0;
}

FILE* ChoiceFileToStatistics()
{
    FILE *fl;
    int choice = 0;

    printf("Please choise what files you want to statistics(1 or 2): \n1. All files, \n2. Only FPC internal files\n");
    while (true)
    {
        scanf("%d", &choice);
        if (choice == 1)
        {
            fl = OpenFile(ALL_FILES_LOG, "r");
            break;
        }
        else if (choice == 2)
        {
            fl = OpenFile(FPC_FILES_LOG, "r");
            break;
        }
        else
        {
            printf("Input a wrong choice, please choice again!\n");
        }
    }
    return fl;
}

void GetAllFilesPath()
{
    char command[500] = COMMAND;
    strcat(command, ALL_FILES_PATH_LOG);
    system(command);

    GetAllFilesNamePath();
    GetFPCFilesNamePath();
}

void GetAllFilesNamePath()
{
    int path_len = 0;
    long count = 0;
    FILE *fl;
    char filename[MAX_PATH_LEN] = { 0 };
    char *allfilepath[5000] = { 0 };

    fl = OpenFile(ALL_FILES_PATH_LOG, "r");
    while (fgets(filename, MAX_PATH_LEN, fl) != NULL)
    {
        TrimString(filename);
        path_len = strlen(filename);

        if (IsCFile(filename, path_len) || IsCSFile(filename, path_len))
        {
            char *temp = (char*)calloc(strlen(filename), sizeof(char));
            memcpy(temp, filename, path_len);
            temp[path_len] = '\0';
            allfilepath[count++] = temp;
            temp = NULL;
            free(temp);
        }
    }
    fclose(fl);

    WriteFileNameToLog(allfilepath, count, 1);
}

void GetFPCFilesNamePath()
{
    int path_len = 0;
    long count = 0;
    FILE *fl, *fp;
    char filename[MAX_PATH_LEN] = { 0 }, str[MAX_PATH_LEN] = { 0 }, *line = NULL;
    char *fpcfilepath[5000] = { 0 };

    fl = OpenFile(ALL_FILES_PATH_LOG, "r");
    while (fgets(filename, MAX_PATH_LEN, fl) != NULL)
    {
        TrimString(filename);
        path_len = strlen(filename);

        if (IsCFile(filename, path_len) || IsCSFile(filename, path_len))
        {
            fp = OpenFile(filename, "r");
            if (fgets(str, MAX_PATH_LEN, fp) != NULL)
            {
                line = TrimString(str);
                if ((line[0] == '/') && ((line[1] == '*') || (line[1] == '/')))
                {
                    if (IsFPCFile(fp, line))
                    {
                        char *temp = (char*)calloc(strlen(filename), sizeof(char));
                        memcpy(temp, filename, path_len);
                        temp[path_len] = '\0';
                        fpcfilepath[count++] = temp;
                        temp = NULL;
                        free(temp);
                    }
                }
            }
            fclose(fp);
        }
    }
    fclose(fl);

    WriteFileNameToLog(fpcfilepath, count, 2);
}

int AllFpcFilesCount(FILE *fp)
{
    if (fp == NULL)
    {
        printf("%s is NULL", fp);
        exit(0);
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
        printf("%s can not be opened", file);
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

    char *temp = line;
    // left trim
    for (int i = 0; i < MAX_PATH_LEN && line[i] != '\0'; i++)
    {
        if (isspace(line[i]))
        {
            temp = line + 1;
            line = line + 1;
            i--;
        }
        else
        {
            break;
        }
    }

    //right trim
    char *str = temp;
    for (int i = strlen(temp); i >= 0; i--)
    {
        if (temp[i - 1] > 127 || temp[i - 1] < 0)
        {
            temp[i - 1] = '\0';
        }
        else if ((temp[i - 1] == '\n') ||               // remove '\n'
            (isspace(temp[i - 1])) ||                   // remove space
            ((temp[i - 1] > 255 || temp[i - 1] < 0)))   // remove non-ascii character
        {
            temp[i - 1] = '\0';
        }
        else
        {
            break;
        }
    }

    return str;
}

bool IsCFile(char *filename, int len)
{
    bool status = false;
    if (filename == NULL)
    {
        printf("%s is NULL string!\n", filename);
        exit(1);
    }

    if ((filename[len - 2] == '.') && ((filename[len - 1]) == 'h'))
    {
        h_file_number++;
        status = true;
    }
    else if (((filename[len - 2] == '.') && ((filename[len - 1]) == 'c')))
    {
        c_file_number++;
        status = true;
    }
    else if (filename[len - 1] == 'p' && filename[len - 2] == 'p' &&
        filename[len - 3] == 'c' && filename[len - 4] == '.')
    {
        cpp_file_number++;
        status = true;
    }

    return status;
}

bool IsCSFile(char *filename, int len)
{
    bool status = false;
    if (filename == NULL)
    {
        printf("%s is NULL string!\n", filename);
        exit(1);
    }

    if (filename[len - 1] == '.' && filename[len - 2] == 'c' && filename[len - 3] == 's')
    {
        cs_file_number++;
        status = true;
    }

    return status;
}

bool IsFPCFile(FILE *fp, char *line)
{
    int i = 0;
    bool status = false;
    if ((fp == NULL) || (line == NULL))
    {
        printf("%s or %s is NULL string!\n", fp, line);
        exit(1);
    }

    char str[MAX_PATH_LEN] = { 0 };
    while ((fgets(str, MAX_PATH_LEN, fp) != NULL) && (i < 10))
    {
        line = TrimString(str);
        if (strstr(line, "Fingerprint Cards") != NULL)
        {
            status = true;
            break;
        }
        i++;
    }

    return status;
}

void IgnoreCommentType1(FILE *fp, char *line)
{
    if ((fp == NULL) || (line == NULL))
    {
        printf("%s or %s is NULL string!\n", fp, line);
        exit(1);
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
