#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>


int byteCount(FILE* fp)
{
    int byte_count = 0;
    while(fgetc(fp) != EOF)
    {
        byte_count++;
    }
    return byte_count;
}

int lineCount(FILE* fp)
{
    int line_count = 0;
    int ch;
    if(fgetc(fp)!= EOF)
        line_count++;
    while((ch = fgetc(fp)) != EOF)
    {
        if(ch == '\n')
        line_count++;
    }
    return line_count;
}

int wordCount(FILE* fp)
{
    int ch, lastch;
    bool in_word = false;
    int word_count = 0;
    while((ch = fgetc(fp)) != EOF)
    {
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
    return word_count;
}

int main(int argc, char ** argv)
{
    int c;
    FILE* fp = fopen(argv[1], "r");
    if(fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    c = getopt(argc, argv + 1, "-lcw");
    switch(c)
    {
        case 'l':
        fprintf(stdout, "行数：%d\n", lineCount(fp));
        break;

        case 'c':
        fprintf(stdout, "字节数：%d\n", byteCount(fp));
        break;

        case 'w':
        fprintf(stdout, "单词数：%d\n", wordCount(fp));
        break;

        default:
        break;
    }

    fclose(fp);
    exit(0);
}