#ifndef _PARSER_FILE_H
#define _PARSER_FILE_H

#include <stdio.h>
#include <stdbool.h>

// Recursively find all folder and file names in a specified folder
// and saved into file List_All_Files_Path
void GetAllFilesPath();

// From List_All_Files_Path, retrieve all the C or CS file into file All_Files_Path
void GetAllFilesNamePath();

// From List_All_Files_Path, retrieve all the C or CS file into file FPC_Files_Path
void GetFPCFilesNamePath();

// From FPC_Files_Path, summarize all FPC file quantity
int AllFpcFilesCount(FILE *fp);

// Prompt that let user to choice what type files to do statistics
// Two choices: FPC interanl files, and all kinds of files
FILE *ChoiceFileToStatistics();

#endif // !_PARSER_FILE_H
