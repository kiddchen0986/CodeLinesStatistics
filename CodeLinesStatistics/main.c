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
        else if (IsCSFile(filename))
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
