#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int total_line_count = 0, code_line_count = 0, comment_line_count = 0, space_line_count = 0;
    int ch, lastch;
    bool is_comment_type1 = false, is_comment_type2 = false, is_space = true;
    char *res;
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    while ((ch = fgetc(fp)) != EOF)
    {
        if (!is_comment_type1 && lastch == '/' && ch == '/')
        {

            is_comment_type1 = true;
            is_space = false;
        }
        if (!is_comment_type2 && lastch == '/' && ch == '*')
        {
            is_comment_type2 = true;
            is_space = false;
        }
        else if (!isspace(ch) || is_comment_type1)
        {
            is_space = false;
        }
        if (ch == '\n')
        {
            total_line_count++;
            if (is_comment_type1 || is_comment_type2)
            {
                comment_line_count++;
            }
            if (is_space)
            {
                space_line_count++;
            }

            is_comment_type1 = false;
            is_space = true;
        }
        if (lastch == '*' && ch == '/')
        {
            comment_line_count++;
            is_comment_type2 = false;
        }

        lastch = ch;
    }
    if (lastch == '\n' || !isspace(lastch))
    {
        total_line_count++;
        if (is_comment_type1 || is_comment_type2)
        {
            comment_line_count++;
        }
        if (is_space)
        {
            space_line_count++;
        }
    }
    code_line_count = total_line_count - comment_line_count - space_line_count;
    fprintf(stdout, "总行数：%d\t代码行数：%d\t注释行数：%d\t空行数：%d\n", total_line_count, code_line_count, comment_line_count, space_line_count);
    fclose(fp);
    exit(0);
}