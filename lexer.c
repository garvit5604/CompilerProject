#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "lexer.h"
#include "symbol_table.h"

extern int errno;

Symbol *symbol_table;
TokenMap *token_map;
LexemeArray *lexeme_array;

int readBuffer(char buffer[4097], int fd)
{
    int sz = read(fd, buffer, 4093);
    buffer[sz] = '\0';
    buffer[sz + 1] = '\0';
    buffer[sz + 2] = '\0';
    buffer[sz + 3] = '\0';
    return sz;
}

void tokenise(Symbol *symbol, LexemeArray *lexeme_array, TokenMap *mapping, char *lexemebegin, char *forward, char *token, int line_no)
{
    int length = forward - lexemebegin;
    if ((length > 20 && strcmp(token, "TK_ID") == 0) || (length > 30 && strcmp(token, "TK_FUNID") == 0))
    {
        printf("Error: Variable Identifier is longer than the prescribed length of 20 characters \n \n");
        return;
    }
    char temp[length + 1];
    for (int i = 0; i < length; i++)
        temp[i] = lexemebegin[i];
    temp[length] = '\0';

    insert(symbol, lexeme_array, mapping, temp, token, line_no);
    printf(" Line_Number:%d, Lexeme:%s, Token:%s,Length:%d\n\n", line_no, temp, token, length);
    
}

tokenInfo getNextToken(twinBuffer B)
{
    char buffer1[4097], buffer2[4097];

    int fd = open(testcaseFile, O_RDONLY);

    if (fd == -1)
    {
        printf("Error Number % d\n", errno);
        perror("Program");
    }

    int sz = readBuffer(buffer1, fd);
    char *lexemebegin = buffer1;
    char *forward = buffer1;
    int line_no = 1;
    int state = 0;
    char c;
    int flag = 0;
    while (1)
    {
        c = *(forward);
        if (c == '\0' && flag)
        {
            break;
        }
        if (forward == (buffer1 + 4096))
        {
            sz = readBuffer(buffer2, fd);
            forward = buffer2;
        }
        if (forward == (buffer2 + 4096))
        {
            sz = readBuffer(buffer1, fd);
            forward = buffer1;
        }
        switch (state)
        {
        case 100: // error state
        {
            // give begin to forward-2 as error
            char temp[forward - 1 - lexemebegin];
            strncpy(temp, lexemebegin, forward - 1 - lexemebegin);
            temp[forward - 1 - lexemebegin] = '\0';
            printf("Error token %s at line %d \n\n", temp, line_no);
            state = 0;
            forward--;
            flag = 1;
        }
        break;
        case 0:
        {
            flag = 0;
            while (*(forward) == ' ' || *(forward) == '\t'|| *(forward) == '\n')
            {
                if(*(forward)=='\n')
                    line_no++;
                forward++;
            }
            lexemebegin = forward;
            c = *(forward);
            if (c == '<')
            {
                state = 1;
            }
            else if (c == '>')
                state = 7;
            else if (c == '!')
                state = 10;
            else if (c == '=')
                state = 12;
            else if (c == '@')
                state = 14;
            else if (c == '%')
                state = 33;
            else if (c == '&')
                state = 17;
            else if (c == '~')
                state = 20;
            else if (c >= 'b' && c <= 'd')
                state = 21;
            else if (c == 'a' || (c >= 'e' && c <= 'z'))
                state = 25;
            else if (c == '#')
                state = 27;
            else if (c >= '0' && c <= '9')
                state = 28;
            else if (c == '%')
                state = 33;
            else if (c == '_')
                state = 38;
            else if (c == ':')
                state = 42;
            else if (c == '.')
                state = 43;
            else if (c == ';')
                state = 44;
            else if (c == '/')
                state = 45;
            else if (c == '*')
                state = 46;
            else if (c == '-')
            {
                state = 47;
            }
            else if (c == '+')
                state = 48;
            else if (c == ',')
                state = 49;
            else if (c == ')')
                state = 50;
            else if (c == '(')
                state = 51;
            else if (c == ']')
                state = 52;
            else if (c == '[')
                state = 53;
            else if (c == '\0')
            {
                flag = 1;
                break;
            }
            else
                state = 99;
            forward++;
        }
        break;
        case 33: // comment handler
        {
            c = *(forward);
            if (c == '\n')
                state = 58;
            else
                state = 33;
            forward++;
        }
        break;

        case 1:
        {
            c = *(forward);
            if (c == '-')
                state = 2;
            else if (c == '=')
                state = 5;
            else
                state = 6;
            forward++;
        }
        break;

        case 2:
        {
            c = *(forward);
            if (c == '-')
                state = 3;
            else
                state = 56;
            forward++;
        }
        break;

        case 3:
        {
            c = *(forward);
            if (c == '-')
                state = 4;
            else
                state = 100;
            forward++;
        }
        break;
        case 7:
        {
            c = *(forward);
            if (c == '=')
                state = 8;
            else
                state = 9;
            forward++;
        }
        break;
        case 10:
        {
            c = *(forward);
            if (c == '=')
                state = 11;
            else
                state = 100;
            forward++;
        }
        break;
        case 12:
        {
            c = *(forward);
            if (c == '=')
                state = 13;
            else
                state = 100;
            forward++;
        }
        break;
        case 14:
        {
            c = *(forward);
            if (c == '@')
                state = 15;
            else
                state = 100;
            forward++;
        }
        break;

        case 15:
        {
            c = *(forward);
            if (c == '@')
                state = 16;
            else
                state = 100;
            forward++;
        }
        break;

        case 17:
        {
            c = *(forward);
            if (c == '&')
                state = 18;
            else
                state = 100;
            forward++;
        }
        break;

        case 18:
        {
            c = *(forward);
            if (c == '&')
                state = 19;
            else
                state = 100;
            forward++;
        }
        break;

        case 20:
        {
            // tokenize TK_MINUS, -
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_NOT", line_no);
            state = 0;
            flag = 1;
        }
        break;

        case 21:
        {
            c = *(forward);
            if (c >= '2' && c <= '7')
                state = 22;
            else if (c >= 'a' && c <= 'z')
                state = 25;
            else
                state = 26;
            forward++;
        }
        break;

        case 22:
        {
            c = *(forward);
            if (c >= 'b' && c <= 'd')
                state = 22;
            else if (c >= '2' && c <= '7')
                state = 23;
            else
                state = 24;
            forward++;
        }
        break;

        case 23:
        {
            c = *(forward);
            if (c >= '2' && c <= '7')
                state = 23;
            else
                state = 24;
            forward++;
        }
        break;

        case 25:
        {
            c = *(forward);
            if (c >= 'a' && c <= 'z')
                state = 25;
            else
                state = 26;
            forward++;
        }
        break;

        case 27:
        {
            c = *(forward);
            if (c >= 'a' && c <= 'z')
                state = 54;
            else
                state = 100;
            forward++;
        }
        break;

        case 54:
        {
            c = *(forward);
            if (c >= 'a' && c <= 'z')
                state = 54;
            else
                state = 55;
            forward++;
        }
        break;

        case 28:
        {
            c = *(forward);
            if (c == '.')
                state = 29;
            else if (c >= '0' && c <= '9')
                state = 28;
            else
                state = 30;
            forward++;
        }
        break;

        case 29:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 31;
            else
                state = 57;
            forward++;
        }
        break;

        case 31:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 32;
            else
                state = 100;
            forward++;
        }
        break;

        case 32:
        {
            c = *(forward);
            if (c == 'E')
                state = 34;
            else
                state = 37;
            forward++;
        }
        break;

        case 34:
        {
            c = *(forward);
            if (c == '+' || c == '-')
                state = 35;
            else if (c >= '0' && c <= '9')
                state = 36;
            else
                state = 100;
            forward++;
        }
        break;

        case 35:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 36;
            else
                state = 100;
            forward++;
        }
        break;

        case 36:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 37;
            else
                state = 100;
            forward++;
        }
        break;

        case 38:
        {
            c = *(forward);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                state = 39;
            else
                state = 100;
            forward++;
        }
        break;

        case 39:
        {
            c = *(forward);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                state = 39;
            else if (c >= '0' && c <= '9')
                state = 41;
            else
                state = 40;
            forward++;
        }
        break;

        case 41:
        {
            c = *(forward);
            if (c >= '0' && c <= '9')
                state = 41;
            else
                state = 40;
            forward++;
        }
        break;
            /*




            final states






            */

        case 4:
        {
            // tokenize TK_ASSIGNOP, <---
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_ASSIGNOP", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 6:
        {
            // tokenize TK_LT, < (single retract)
            forward--;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_LT", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 5:
        {
            // tokenize, TK_LE, <=
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_LE", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 56:
        {
            // tokenize TK_LT, < (double retract)
            forward -= 2;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_LT", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 57:
        {
            // tokenize TK_RNUM, ptr (double retraction state)
            forward -= 2;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_RNUM", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 40:
        {

            forward--;
            int len = forward - lexemebegin;
            char temp[len + 1];
            for (int i = 0; i < len; i++)
                temp[i] = lexemebegin[i];
            temp[len] = '\0';
            if (strcmp(temp, "_main") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_MAIN", line_no); // check for _main and tokenise it
            else
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_FUNID", line_no); // tokenize TK_FUNID, ptr (single retraction)
            state = 0;
            flag = 1;
        }
        break;
        case 42:
        {
            // tokenize TK_COLON, :
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_COLON", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 43:
        {
            // tokenize TK_DOT, .
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_DOT", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 44:
        {
            // tokenize TK_SEM, ;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_SEM", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 45:
        {
            // tokenize TK_DIV, /
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_DIV", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 46:
        {
            // tokenize TK_MUL, *
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_MUL", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 47:
        {
            // tokenize TK_MINUS, -
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_MINUS", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 48:
        {
            // tokenize TK_PLUS, +
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_PLUS", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 49:
        {
            // tokenise TK_COMMA, ,
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_COMMA", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 50:
        {
            // tokenise TK_CL, )
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_CL", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 51:
        {
            // tokenise TK_OP, (
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_OP", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 52:
        {
            // tokenise TK_SQR, ]
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_SQR", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 53:
        {
            // tokenise TK_SQL, [
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_SQL", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 37:
        {
            // tokenise TK_RNUM, ptr
            forward--;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_RNUM", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 30:
        {
            // tokenize TK_NUM, ptr (single retraction)
            forward--;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_NUM", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 55:
        {
            // tokenize TK_RUID, ptr
            forward--;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_RUID", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 8:
        {
            forward = forward;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_GE", line_no);
            state = 0;
            flag = 1;
        }
        break;

        case 9:
        {
            forward = forward - 1;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_GT", line_no);
            state = 0;
            flag = 1;
        }
        break;

        case 11:
        {
            forward = forward;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_NE", line_no);
            state = 0;
            flag = 1;
        }
        break;

        case 13:
        {
            forward = forward;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_EQ", line_no);
            state = 0;
            flag = 1;
        }
        break;

        case 16:
        {
            forward = forward;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_OR", line_no);
            state = 0;
            flag = 1;
        }
        break;

        case 58:
        {
            forward = forward;
            line_no++;
            state = 0;
            flag = 1;
        }
        break;

        case 19:
        {
            forward = forward;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_AND", line_no);
            state = 0;
            flag = 1;
        }
        break;

        case 24:
        {
            forward = forward - 1;
            tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_ID", line_no);
            state = 0;
            flag = 1;
        }
        break;

        case 26:
        {
            forward = forward - 1;
            // check for keywords:
            int len = forward - lexemebegin;
            char temp[len + 1];
            for (int i = 0; i < len; i++)
                temp[i] = lexemebegin[i];
            temp[len] = '\0';
            if (strcmp(temp, "with") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_WITH", line_no);
            else if (strcmp(temp, "parameters") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_PARAMETERS", line_no);
            else if (strcmp(temp, "parameter") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_PARAMETER", line_no);
            else if (strcmp(temp, "end") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_END", line_no);
            else if (strcmp(temp, "while") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_WHILE", line_no);
            else if (strcmp(temp, "union") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_UNION", line_no);
            else if (strcmp(temp, "endunion") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_ENDUNION", line_no);
            else if (strcmp(temp, "definetype") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_DEFINETYPE", line_no);
            else if (strcmp(temp, "as") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_AS", line_no);
            else if (strcmp(temp, "type") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_TYPE", line_no);
            else if (strcmp(temp, "global") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_GLOBAL", line_no);
            else if (strcmp(temp, "list") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_LIST", line_no);
            else if (strcmp(temp, "input") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_INPUT", line_no);
            else if (strcmp(temp, "output") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_OUTPUT", line_no);
            else if (strcmp(temp, "int") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_INT", line_no);
            else if (strcmp(temp, "real") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_REAL", line_no);
            else if (strcmp(temp, "endwhile") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_ENDWHILE", line_no);
            else if (strcmp(temp, "if") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_IF", line_no);
            else if (strcmp(temp, "then") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_THEN", line_no);
            else if (strcmp(temp, "endif") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_ENDIF", line_no);
            else if (strcmp(temp, "read") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_READ", line_no);
            else if (strcmp(temp, "write") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_WRITE", line_no);
            else if (strcmp(temp, "return") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_RETURN", line_no);
            else if (strcmp(temp, "call") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_CALL", line_no);
            else if (strcmp(temp, "record") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_RECORD", line_no);
            else if (strcmp(temp, "endrecord") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_ENDRECORD", line_no);
            else if (strcmp(temp, "else") == 0)
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_ELSE", line_no);
            else
                tokenise(symbol, lexeme_array, mapping, lexemebegin, forward, "TK_FIELDID", line_no);
            state = 0;
            flag = 1;
        }
        break;
        case 99:
        {

            state = 100;
            forward++;
        }
        break;
        }
    }
    close(fd);
}

void freeSymbolTable(Symbol* symbol){
    // for(int i=0;i<symbol->size;i++){
    //     free(&symbol->table[i]);
    // }
    free(symbol->table);
    free(symbol);
}

void inti

// tokenInfo lexical_analyzer(char *testcaseFile,Symbol )
// {
//     symbol_table=s;
//     lexeme_array=l;
//     token_map=t;
    
//     // printf("fdadf\n");
//     return getNextToken(symbol_table, lexeme_array, token_map, testcaseFile);
//     // int size = symbol_table->size;
//     // printf("The size of the symbol table is %d\n", size);
//     // Record record;
//     // for (int i = 0; i < size; i++)
//     // {
//     //     // printf("hello\n");
//     //     record = symbol_table->table[i];
//     //     printf("%s ,%s ,%d\n", token_map->buckets[record.token]->lexeme, lexeme_array->buckets[record.lexeme]->lexeme, record.line_no);
//     // }

// }