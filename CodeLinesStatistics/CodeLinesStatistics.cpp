/**
 *       Author: Kidd Chen
 *       Date  : 2022/02/20
 *       Description: Statistics on how many code lines
 *       1. Just count codes written by ourselves
 *       2. Only for effective codes, exclude those comment codes
 *       3. Important to make sure all files are in utf-8 encoding, thus so you please first
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

#define COMMAND "DIR /D E:\\workspace\\mtt\32 /s /B > "
#define ALL_FILES_PATH_LOG "Files_Path.log"
#define FPC_FILE_LOG "FPC_File_Path.log"

static int c_file_number = 0;
static int cpp_file_number = 0;
static int h_file_number = 0;
static int cs_file_number = 0;

void GetAllFilesPath();
void GetFPCFilesPath();
FILE *OpenFile(char *file, char *mode);
char *TrimString(char *line);
bool IsCFile(char *filename, int len);
bool IsCSFile(char *filename, int len);
bool IsFPCFile(FILE *fp, char *str);
int AllFpcFilesCount(FILE *fp);
void IgnoreCommentType1(FILE *fp, char *str);  // i.e. /* ... */, include single line and multi lines
void WriteFPCFileNameToLog(char *fpcfilepath[], int count);
int main()
{
    FILE *fl, *fp;
    int len = 0, sum = 0;
    long int count = 0;
    char filename[MAX_PATH_LEN] = { 0 }, str[MAX_PATH_LEN] = { 0 }, *line = NULL;
    GetAllFilesPath();
    GetFPCFilesPath();

    // Here is to handle FPC internal files, please change to use ALL_FILES_PATH_LOG if want to handle
    // all code files in path, like fl = OpenFile(ALL_FILES_PATH_LOG, "r");
    fl = OpenFile(FPC_FILE_LOG, "r");
    while (fgets(filename, MAX_PATH_LEN, fl) != NULL)
    {
        len = strlen(filename);
        filename[len - 1] = '\0';
        printf("File name: %s\n", filename);

        count = 0;
        fp = OpenFile(filename, "r");
        while (fgets(str, MAX_PATH_LEN, fp) != NULL)
        {
            str[strlen(str) - 1] = '\0';

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

void GetAllFilesPath()
{
    char command[500] = COMMAND;
    strcat(command, ALL_FILES_PATH_LOG);
    system(command);
}

void GetFPCFilesPath()
{
    int path_len = 0;
    long count = 0;
    FILE *fl, *fp, *fw;
    char filename[MAX_PATH_LEN] = { 0 }, str[MAX_PATH_LEN] = { 0 }, *line = NULL;
    char *fpcfilepath[5000] = { 0 };

    fl = OpenFile(ALL_FILES_PATH_LOG, "r");
    while (fgets(filename, MAX_PATH_LEN, fl) != NULL)
    {
        path_len = strlen(filename);
        filename[path_len - 1] = '\0';

        if (IsCFile(filename, path_len) || IsCSFile(filename, path_len))
        {
            fp = OpenFile(filename, "r");
            if (fgets(str, MAX_PATH_LEN, fp) != NULL)
            {
                str[strlen(str) - 1] = '\0';
                line = TrimString(str);

                if ((line[0] == '/') && ((line[1] == '*') || (line[1] == '/')))
                {
                    if (IsFPCFile(fp, line))
                    {
                        char *temp = (char*)calloc(strlen(filename), sizeof(char));
                        memcpy(temp, filename, path_len);
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

    WriteFPCFileNameToLog(fpcfilepath, count);
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

    fp = OpenFile(ALL_FILES_PATH_LOG, "r");
    while (fgets(line, MAX_PATH_LEN, fp) != NULL)
    {
        count++;
    }
    fclose(fp);

    return count;
}

void WriteFPCFileNameToLog(char *fpcfilepath[], int count)
{
    FILE *fw;

    fw = OpenFile(FPC_FILE_LOG, "w");
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
        if ((temp[i - 1] == '\n') || (isspace(temp[i - 1])))  // remove '\n' and space
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

    if ((filename[len - 3] == '.') && ((filename[len - 2]) == 'h'))
    {
        h_file_number++;
        status = true;
    }
    else if (((filename[len - 3] == '.') && ((filename[len - 2]) == 'c')))
    {
        c_file_number++;
        status = true;
    }
    else if (filename[len - 2] == 'p' && filename[len - 3] == 'p' &&
        filename[len - 4] == 'c' && filename[len - 5] == '.')
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

    if (filename[len - 2] == 's' && filename[len - 3] == 'c' && filename[len - 4] == '.')
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
        str[strlen(str) - 1] = '\0';

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
                ((line[strlen(line) - 2] == '*') && (line[strlen(line) - 1] == '/')))   // at line's ends
            {
                break;
            }
        }
    }
}
