#ifndef _TOOLS_H
#define _TOOLS_H

#include"stdio.h"
#include"stdbool.h"

// open a file with mode, return pointer to file
FILE *OpenFile(char* file, char* mode);

// Trim the string to remove space of before and after
char *TrimString(char *line);

// Write all files name into specified file, here all files means C and CS files
// parameters
//    fpcfilepath : pointer to file name
//    count       : file quantity
//    type        : two types, 1 means all kinds of file, 2 means only FPC files
void WriteFileNameToLog(char *fpcfilepath[], int count, int type);

// get C, CS, FPC files
bool IsCFile(char *filename);
bool IsCSFile(char *filename);
bool IsFPCFile(char *filename);

// Handle one comment type, i.e. /* ... */
// the end '*/', includes either in the same line or another line
void IgnoreCommentType1(FILE *fp, char *str);  

#endif
