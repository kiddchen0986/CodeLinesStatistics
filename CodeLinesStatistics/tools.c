#include"config.h"
#include"stdio.h"
#include"stdbool.h"

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
        else if ((line[i - 1] == '\n') || (isspace(line[i - 1])))   // remove '\n' and space
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

void WriteFileNameToLog(char *fpcfilepath[], int count, int type)
{
    FILE *fw;

    fw = (type == 1 ? OpenFile(ALL_FILES_LOG, "w") : OpenFile(FPC_FILES_LOG, "w"));
    for (int i = 0; i < count; i++)
    {
        fputs(fpcfilepath[i], fw);
        fputc('\n', fw);
    }
    fclose(fw);
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
