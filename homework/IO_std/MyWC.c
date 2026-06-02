#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>


int main(int argc, char ** argv)
{
    int word_count = 0, byte_count = 0, line_count = 0;
    bool in_word = false;
    int ch;
    int lastch;
    FILE* fp = fopen(argv[1], "r");
    if(fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    if(fgetc(fp) != EOF)
        {
            line_count ++;
            byte_count ++;
        }
    while((ch = fgetc(fp)) != EOF)
    {
        byte_count++;
        if(ch == '\n')
            line_count ++;
        if(isspace(ch) || lastch == '.' || lastch == '?' || lastch == '!')
        {
            in_word = false;
        }
        else if(!in_word)
        {
            word_count ++;
            in_word = true;
        }        
        lastch = ch;
    }
    
    fprintf(stdout, "字节数：%d\t 单词数：%d\t 行数：%d\n", byte_count, word_count, line_count);
    fclose(fp);
    exit(0);
}