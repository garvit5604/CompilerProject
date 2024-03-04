#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "lexer.h"
#include "parser.h"

grammar G;

// Funtion to find the location of the terminal or non terminal in the ter
int find_location(char *term)
{
    for (int i = 0; i < TERMS_SIZE; i++)
    {
        if (strcmp(grammarTerms[i], term) == 0)
        {
            return i;
        }
    }
    return -1;
}

// Function to create a new stack
Stack *newStack()
{
    Stack *s = (Stack *)malloc(sizeof(Stack));
    if (s != NULL)
        s->top = -1;
    return s;
}

// Function to return the top of the stack
parseTree top(Stack *stack)
{
    int top = stack->top;
    return stack->data[top];
}

// Function to pop the top of the stack
int pop(Stack *stack)
{
    if (stack->top == -1)
        return 0;
    (stack->top)--;
    return 1;
}

// Function to check whether the stack is empty or not
int isEmpty(Stack *stack)
{
    if (stack->top == -1)
        return 1;
    else
        return 0;
}

// Function to push the element which is our non terminal into the stack
int push(Stack *s, parseTree e)
{
    if (s->top == STACK_SIZE - 1)
        return 0;
    s->data[++(s->top)] = e;
    return 1;
}

// Function to print all the elements of the stack
void print_stack(Stack *stack)
{
    for (int i = 0; i <= stack->top; i++)
    {
        printf("%d\n", stack->data[i]->current);
    }
}

// Function to create a new parsernode
parseTree createNewNode()
{
    // printf("inside ");
    parseTree node = (parseTree)malloc(sizeof(struct parseTree));
    node->parent = NULL;
    node->child = NULL;
    node->sizeofChild = 0;
    node->lineNo = 0;
    // printf("inside createNewNode");
    // for(int i=0;i<5;i++)
    // node->child[i]=NULL;
    // printf("outside");
    return node;
}

// Function to parse the given token
void parseFile(int lineNo, char *lexeme, char *token, table T, Stack *s, grammar G)
{
    // printf("%s ", token);
    if (strcmp(token, "TK_COMMENT") == 0)
        return;
    int flag = 0;
    while (flag == 0)
    {

        parseTree topNode = top(s);
        // printf("%d ", topNode->current);
        int indx = find_location(token);
        if (topNode->current > NONTERMINALS && strcmp(grammarTerms[topNode->current], token) == 0)
        {
            printf("Matched %s\n", token);
            // topNode->tokenName=token;
            topNode->child = NULL;
            topNode->sizeofChild = 0;
            topNode->lineNo = lineNo;
            topNode->lexeme = lexeme;
            pop(s);
            flag = 1;
            break;
        }
        else if (topNode->current > NONTERMINALS && strcmp(grammarTerms[topNode->current], token) != 0)
        {
            printf("Error with the token %s\n", token);
            pop(s);
            return;
        }
        else
        {
            // int *temp;
            int rule = T->parserTable[topNode->current - 1][indx - NONTERMINALS];
            // temp = T->parserTable[topNode->current - 1][indx - NONTERMINALS];
            // printf("%d\n", rule);
            // if (T->parserTable[topNode->current - 1][indx - NONTERMINALS] != NULL)
            if (rule == -1)
            {
                printf("Erro with token %s\n", token);
                return;
            }
            else if (rule == SYN)
            {
                pop(s);
            }
            else if (rule != -1)
            {
                printf("Output: ");
                for (int j = 0; j < 10; j++)
                {
                    // if(temp[j] == -1)
                    if ((G->Grammar)[rule][j] == -1)
                    {
                        printf("\n");
                        break;
                    }
                    if (j == 0)
                    {
                        printf("<%s> ===> ", grammarTerms[(G->Grammar)[rule][j]]);
                        // printf("<%s> ===> ", grammarTerms[temp[j]]);
                    }
                    else
                    {
                        // if (grammarTerms[temp[j]][0] == 'T' || temp[j] == EPLS)
                        if (grammarTerms[G->Grammar[rule][j]][0] == 'T' || G->Grammar[rule][j] == EPLS)
                            printf("%s ", grammarTerms[(G->Grammar)[rule][j]]);
                        // printf("%s ", grammarTerms[temp[j]]);
                        else
                            printf("<%s> ", grammarTerms[(G->Grammar)[rule][j]]);
                        // printf("<%s> ", grammarTerms[temp[j]]);
                    }
                }
                pop(s);
                int top = topNode->current;
                int flag2 = 0;
                for (int i = 10; i >= 1; i--)
                {
                    // if (temp[i] == -1)
                    if (G->Grammar[rule][i] == -1)
                    {
                        flag2 = 1;
                        topNode->child = (parseTree *)malloc((i - 1) * sizeof(struct parseTree));
                        // printf("%d ", top);
                        topNode->sizeofChild = i - 1;
                    }
                    // if (flag2 == 1 && temp[i] != -1)
                    if (flag2 == 1 && G->Grammar[rule][i] != -1)
                    {
                        // printf("Outside loop %d ", top);
                        // if (temp[i] == EPLS)
                        if (G->Grammar[rule][i] == EPLS)
                        {
                            parseTree tempNode = createNewNode();
                            // printf("%d ", top);
                            tempNode->child = NULL;
                            tempNode->parent = topNode;
                            tempNode->current = EPLS;
                            topNode->child[i - 1] = tempNode;
                        }
                        else
                        {
                            // printf("%d ", G->Grammar[rule][i]);
                            parseTree tempNode = createNewNode();
                            // printf("Hello%d ", top);
                            // printf("Hello%d ", top);
                            // printf("Hello%d ", top);
                            tempNode->current = G->Grammar[rule][i];
                            // tempNode->current = temp[i];
                            // printf("hello");
                            tempNode->parent = topNode;
                            // printf("Hello%d",i-1);
                            topNode->child[i - 1] = tempNode;
                            // printf("Hello%d ", top);
                            // if (push(s, tempNode))
                            //     printf("True");
                            // else
                            //     printf("False");
                            push(s, tempNode);
                        }
                    }
                }
                print_stack(s);
            }
        }
    }
}

// Function to parse the testcase File
parseTree parseInputSourceCode(char *testcaseFile, table T)
{
    Stack *s = newStack();
    parseTree root = createNewNode();
    root->parent = NULL;
    root->current = 1;
    push(s, root);
    LexemeArray *lexeme_array = initialize_lexeme_array();
    Symbol *symbol_table = initialize_symbol_table();
    TokenMap *token_map = initialize_token_map();
    insertKeywords(symbol_table, lexeme_array, token_map);
    FILE *file = open(testcaseFile,"r");
    //twinBuffer initialize
    lexical_intializer(symbol_table,lexeme_array,token_map);
    while(){
        getNextToken(twinBuffer);
        parseFile()
    }
    // FILE *f = fopen(testcaseFile, "r");
    // // printf("HelloBye");
    // char buffer[1024];
    // while (fgets(buffer, 1024, f))
    // {
    //     // printf("Hello");
    //     char *token = strtok(buffer, " ");
    //     // printf("%s", token);
    //     while (token)
    //     {
    //         parseFile(token, T, s, G);
    //         token = strtok(NULL, " ");
    //     }
    // }
    // fclose(f);
    freeSymbolTable(symbol_table);
    freeHashSet(lexeme_array);
    freeHashSet(token_map);
    return root;
}

// void readFile()
// {
//     FILE *f = fopen("program.txt", "r");
//     if (f == NULL)
//     {
//         perror("Error in opening the file");
//         return;
//     }
//     char buffer[1024];
//     while (fgets(buffer, 1024, f))
//     {
//         // printf("Hello");
//         char *token = strtok(buffer, " ");
//         while (token)
//         {
//             parseFile(token, T, s, G);
//             printf("%s", token);
//             token = strtok(NULL, " ");
//         }
//     }
//     fclose(f);
// }

// Recursive function which helps in printing the parse tree
void outputTree(parseTree root, FILE *fd)
{
    int sz = root->sizeofChild;
    for (int i = 0; i < sz - 1; i++)
    {
        // printf("%d\n",root->current);
        outputTree(root->child[i], fd);
    }
    char leaf[4];
    char dummy[5] = "----";
    if (sz == 0)
    {
        strcpy(leaf, "yes");
    }
    else
    {
        strcpy(leaf, "no");
        // root->lexeme = dummy;
    }

    if (root->current == 1)
    {
        fprintf(fd, "%s %d %s %s %s %s %s\n", "lexeme", root->lineNo, grammarTerms[root->current], "----", "$", leaf, grammarTerms[root->current]);
    }
    else
    {
        if (strcmp(grammarTerms[root->current], "TK_NUM") == 0)
            fprintf(fd, "%s %d %s %d %s %s %s\n", "lexeme", root->lineNo, grammarTerms[root->current], atoi("123"), grammarTerms[root->parent->current], leaf, grammarTerms[root->current]);
        else if (strcmp(grammarTerms[root->current], "TK_RNUM") == 0)
            fprintf(fd, "%s %d %s %f %s %s %s\n", "lexeme", root->lineNo, grammarTerms[root->current], atof("123.34"), grammarTerms[root->parent->current], leaf, grammarTerms[root->current]);
        else
            fprintf(fd, "%s %d %s %s %s %s %s\n", "lexeme", root->lineNo, grammarTerms[root->current], "----", grammarTerms[root->parent->current], leaf, grammarTerms[root->current]);
    }

    if (sz >= 1)
        outputTree(root->child[sz - 1], fd);
}

// Function to print the parse tree
void printParseTree(parseTree root, char *outfile)
{
    FILE *f = fopen(outfile, "w");
    if (f == NULL)
    {
        printf("Error in opening the file");
        return;
    }
    outputTree(root, f);
    fclose(f);
}

// Function to print Grammar
void print_grammar(grammar G)
{
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if ((G->Grammar)[i][j] == -1)
            {
                printf("\n");
                break;
            }
            if (j == 0)
            {
                printf("<%s> ===> ", grammarTerms[(G->Grammar)[i][j]]);
            }
            else
            {
                if (grammarTerms[G->Grammar[i][j]][0] == 'T' || G->Grammar[i][j] == EPLS)
                    printf("%s ", grammarTerms[(G->Grammar)[i][j]]);
                else
                    printf("<%s> ", grammarTerms[(G->Grammar)[i][j]]);
            }
        }
    }
}

// Function to print the first set of grammar terms
void print_first_set(FirstAndFollow F, grammar G)
{
    for (int i = 0; i < TERMS_SIZE; i++)
    {
        printf("%s ===> ", grammarTerms[i]);
        for (int j = 0; j < 20; j++)
        {
            if (F->first[i][j] == 0)
                break;
            else
                printf("%s, ", grammarTerms[F->first[i][j]]);
        }
        printf("\n");
    }
}

// FUnction to print the follow set of non terminals
void print_follow_set(FirstAndFollow F, grammar G)
{
    for (int i = 1; i <= NONTERMINALS; i++)
    {
        printf("%s ===> ", grammarTerms[i]);
        for (int j = 0; j < 100; j++)
        {
            if (F->follow[i][j] == 0)
                break;
            else if (F->follow[i][j] == -1)
                printf("$, ");
            else
                printf("%s, ", grammarTerms[F->follow[i][j]]);
        }
        printf("\n");
    }
}

// Function to check whether the given term is already present in the set or not
int check_term(int *temp_array, int n)
{
    int i = 0, flag = 0;
    for (i = 0; temp_array[i] != 0; i++)
    {
        if (temp_array[i] == n)
        {
            flag = 1;
            break;
        }
    }
    return flag; // if b is in array a
}

// Computes the FIRST set of a term
void First(int n, grammar G, int temp_array[100], FirstAndFollow F)
{
    int j = 0;
    for (int i = 0; i < 20; i++)
    {
        if (temp_array[i] == 0)
        {
            j = i;
            break;
        }
    }
    if (n > NONTERMINALS)
    {
        temp_array[j] = n;
    }
    else
    {
        for (int i = 0; i < GRAMMAR_SIZE; i++)
        {
            if (G->Grammar[i][0] == n)
            {
                if (G->Grammar[i][1] > NONTERMINALS)
                {
                    if (!check_term(temp_array, G->Grammar[i][1]))
                    {
                        temp_array[j++] = G->Grammar[i][1];
                    }
                }
                else
                {
                    int flag = 0;
                    for (int k = 1; k < 10; k++)
                    {
                        if (G->Grammar[i][k] == -1)
                            break;
                        // int sub_temp_array[100] = {0};
                        flag = 0;
                        First(G->Grammar[i][k], G, F->first[G->Grammar[i][k]], F);
                        for (int l = 0; F->first[G->Grammar[i][k]][l] != 0; l++)
                        {
                            if (F->first[G->Grammar[i][k]][l] == EPLS)
                                flag = 1;
                            else
                            {
                                if (!check_term(temp_array, F->first[G->Grammar[i][k]][l]))
                                {
                                    temp_array[j++] = F->first[G->Grammar[i][k]][l];
                                    // printf("%d ",F->first[G->Grammar[i][k]][l]);
                                }
                            }
                        }
                        if (flag != 1)
                            break;
                    }
                    if (flag == 1)
                    {
                        temp_array[j++] = EPLS;
                    }
                }
            }
        }
    }
}

// Computes the FOLLOW set of a term
void Follow(int n, grammar G, int temp_array[100], FirstAndFollow F, int check_recursion[NONTERMINALS])
{
    int j = 0, k;
    if (check_recursion[n] == 1)
    {
        check_recursion[n] = 0;
        return;
    }
    if (n == 1)
    {
        temp_array[j++] = -1;
    }
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        for (int m = 1; m < 10; m++)
        {
            if (G->Grammar[i][m] == -1)
                break;
            if (G->Grammar[i][m] == n)
            {
                // printf("%d\n", i);
                for (k = m + 1; G->Grammar[i][k] != -1; k++)
                {
                    int flag = 0;
                    for (int l = 0; F->first[G->Grammar[i][k]][l] != 0; l++)
                    {
                        // printf("%d %d %d \n", n, F->first[G->Grammar[i][k]][l], l);
                        if (F->first[G->Grammar[i][k]][l] == EPLS)
                            flag = 1;
                        else
                        {
                            if (!check_term(temp_array, F->first[G->Grammar[i][k]][l]))
                            {
                                temp_array[j++] = F->first[G->Grammar[i][k]][l];
                            }
                        }
                        // printf("hello\n");
                    }
                    if (flag != 1)
                        break;
                }
                // printf("hello2\n");
                if (G->Grammar[i][k] == -1 && G->Grammar[i][0] != n)
                {
                    check_recursion[G->Grammar[i][0]] = 1;
                    Follow(G->Grammar[i][0], G, F->follow[G->Grammar[i][0]], F, check_recursion);
                    check_recursion[G->Grammar[i][0]] = 0;
                    for (int l = 0; F->follow[G->Grammar[i][0]][l] != 0; l++)
                    {
                        if (!check_term(temp_array, F->follow[G->Grammar[i][0]][l]))
                        {
                            temp_array[j++] = F->follow[G->Grammar[i][0]][l];
                        }
                    }
                }
            }
        }
    }
}

// Takes input as G, computes the FIRST and FOLLOW sets
FirstAndFollow ComputeFirstAndFollowSets(grammar G)
{
    FirstAndFollow F = (FirstAndFollow)malloc(sizeof(FirstAndFollow));
    F->first = (int **)malloc(TERMS_SIZE * sizeof(int *));
    for (int i = 0; i < TERMS_SIZE; i++)
    {
        F->first[i] = (int *)malloc(20 * sizeof(int));
        for (int j = 0; j < 20; j++)
        {
            F->first[i][j] = 0;
        }
    }
    for (int i = 1; i < TERMS_SIZE; i++)
    {
        First(i, G, F->first[i], F);
    }
    F->follow = (int **)malloc((NONTERMINALS + 1) * sizeof(int *));
    for (int i = 1; i <= NONTERMINALS; i++)
    {
        F->follow[i] = (int *)malloc(20 * sizeof(int));
        for (int j = 0; j < 20; j++)
        {
            F->follow[i][j] = 0;
        }
    }
    int check_recursion[NONTERMINALS] = {0};
    int count = 0;
    while (count != 5)
    {

        for (int i = 1; i <= NONTERMINALS; i++)
        {
            // printf("%d\n", i);
            // if (F->follow[i][0] == 0)
            Follow(i, G, F->follow[i], F, check_recursion);
        }
        count++;
    }
    return F;
}

// Creates the parse table
void createParseTable(FirstAndFollow F, table T)
{
    int lhs, flag, flag2;
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        lhs = G->Grammar[i][0];
        flag = 0;
        // printf("\n%d - ", lhs);
        for (int j = 1; j < 10; j++)
        {
            flag2 = 0;
            int rhs = G->Grammar[i][j];
            if (rhs == -1)
            {
                break;
            }
            if (rhs == EPLS)
            {
                flag = 1;
                break;
            }
            if (rhs > NONTERMINALS)
            {
                T->parserTable[lhs - 1][rhs - NONTERMINALS] = i;
                // T->parserTable[lhs - 1][rhs - NONTERMINALS]=G->Grammar[i];
                // printf("%d ", rhs);
                break;
            }

            for (int k = 0; k < 20; k++)
            {
                // printf("inside first ");
                if (F->first[rhs][k] == 0)
                    break;
                if (F->first[rhs][k] != EPLS)
                {
                    T->parserTable[lhs - 1][F->first[rhs][k] - NONTERMINALS] = i;
                    // T->parserTable[lhs - 1][F->first[rhs][k] - NONTERMINALS] = G->Grammar[i];
                    // printf("%d ", F->first[rhs][k]);
                }
                if (F->first[rhs][k] == EPLS)
                    flag2 = 1;
            }
            if (flag2 == 0)
                break;
            else if (flag2 == 1 && G->Grammar[i][j + 1] == -1)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 1)
        {
            for (int j = 0; F->follow[lhs][j] != 0; j++)
            {
                int x = F->follow[lhs][j];
                if (x == -1)
                {
                    T->parserTable[lhs - 1][0] = i;
                    // T->parserTable[lhs - 1][0] = G->Grammar[i];
                }
                else
                {
                    T->parserTable[lhs - 1][x - NONTERMINALS] = i;
                    // T->parserTable[lhs - 1][x - NONTERMINALS] = G->Grammar[i];
                    // printf("%d ", x);
                }
            }
        }
        // for (int j = 0; j <= TERMINALS; j++)
        // {
        //     if (T->parserTable[i][j] == -1)
        //         printf("_ ");
        //     else
        //         printf("%d ", T->parserTable[i][j]);
        // }
        // printf("\n");
    }
}

// Prints the parser table
void printTable(table T)
{
    for (int i = 0; i < NONTERMINALS; i++)
    {
        printf("%s ==>", grammarTerms[i + 1]);
        for (int j = 0; j <= TERMINALS; j++)
        {
            if (T->parserTable[i][j] == -1)
                printf("_ ");
            else if (T->parserTable[i][j] == SYN)
                printf("syn ");
            else
                printf("%d ", T->parserTable[i][j]);
        }
        printf("\n");
    }
}

// To initialize the grammar
grammar initialize_grammar()
{
    int temp[GRAMMAR_SIZE][10] = {
        {1, 2, 3, -1},                   // <program> ===> <otherFunctions> <mainFunction>
        {3, 69, 4, 62, -1},              // <mainFunction>===> TK_MAIN <stmts> TK_END
        {2, 5, 2, -1},                   // <otherFunctions>===> <function><otherFunctions>
        {2, 110, -1},                    // <otherFunctions>===> eps
        {5, 58, 6, 7, 80, 4, 62, -1},    // <function>===>TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END
        {6, 75, 71, 72, 73, 8, 74, -1},  // <input_par>===>TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
        {7, 76, 71, 72, 73, 8, 74, -1},  // <output_par>===>TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
        {7, 110, -1},                    // <output_par>===>eps
        {8, 9, 55, 31, -1},              // <parameter_list>===><dataType> TK_ID <remaining_list>
        {9, 10, -1},                     // <dataType>===> <primitiveDatatype>
        {9, 11, -1},                     // <dataType>===> <constructedDatatype>
        {10, 77, -1},                    // <primitiveDatatype>===> TK_INT
        {10, 78, -1},                    // <primitiveDatatype>===> TK_REAL
        {11, 12, 59, -1},                // <constructedDatatype>===> <A> TK_RUID
        {11, 59, -1},                    // <constructedDatatype>===> TK_RUID
        {31, 79, 8, -1},                 // <remaining_list>===>TK_COMMA <parameter_list> | eps
        {31, 110, -1},                   // <remaining_list>===> eps
        {4, 13, 14, 15, 16, -1},         // <stmts>===><typeDefinitions> <declarations> <otherStmts><returnStmt>
        {13, 17, 13, -1},                // <typeDefinitions>===><typeDefinition>  <typedefinitions>
        {13, 18, 13, -1},                // <typeDefinitions>===> <definetypestmt> <typeDefinitions>
        {13, 110, -1},                   // <typeDefinitions>===> eps
        {17, 97, 59, 19, 98, -1},        // <typeDefinition>===>TK_RECORD TK_RUID <fieldDefinitions>TK_ENDRECORD
        {17, 64, 59, 19, 65, -1},        // <typeDefinition>===>TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION
        {19, 20, 20, 21, -1},            // <fieldDefinitions>===> <fieldDefinition><fieldDefinition><moreFields>
        {20, 68, 9, 81, 54, 80, -1},     // <fieldDefinition>===> TK_TYPE <datatype> TK_COLON TK_FIELDID TK_SEM
        {21, 20, 21, -1},                // <moreFields>===><fieldDefinition><moreFields>
        {21, 110, -1},                   // <moreFields>===> eps
        {14, 47, 14, -1},                // <declarations> ===> <declaration><declarations>
        {14, 110, -1},                   // <declarations> ===> eps
        {47, 68, 9, 81, 55, 22, 80, -1}, // <declaration>===> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM
        {22, 81, 70, -1},                // <global_or_not>===>TK_COLON TK_GLOBAL
        {22, 110, -1},                   // <global_or_not>===> eps
        {15, 23, 15, -1},                // <otherStmts>===> <stmt><otherStmts>
        {15, 110, -1},                   // <otherStmts>===>  eps
        {23, 24, -1},                    // <stmt>===> <assignmentStmt>
        {23, 25, -1},                    // <stmt>===> <iterativeStmt>
        {23, 26, -1},                    // <stmt>===> <conditionalStmt>
        {23, 27, -1},                    // <stmt>===> <ioStmt>
        {23, 28, -1},                    // <stmt>===>  <funCallStmt>
        // {23, 18, -1},                            // <stmt>===>  <definetypestmt>
        {24, 29, 109, 30, 80, -1}, // <assignmentStmt>===><SingleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
        {29, 55, 32, -1},          // <singleOrRecId>===>TK_ID <option_single_constructed>
        {32, 33, 34, -1},          //<option_single_constructed>===>  <oneExpansion><moreExpansions>
        {32, 110, -1},
        {34, 33, 34, -1},                         // <moreExpansions>===> <oneExpansion> <moreExpansions>
        {34, 110, -1},                            // <moreExpansions>===> eps
        {33, 82, 54, -1},                         //<oneExpansion>===> TK_DOT TK_FIELDID
        {28, 35, 96, 58, 60, 61, 36, 80, -1},     // <funCallStmt>===><outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM
        {35, 73, 37, 74, 109, -1},                // <outputParameters> ==> TK_SQL <idList> TK_SQR TK_ASSIGNOP
        {35, 110, -1},                            // <outputParameters> ==>  eps
        {36, 73, 37, 74, -1},                     // <inputParameters>===> TK_SQL <idList> TK_SQR
        {25, 63, 84, 38, 85, 23, 15, 83, -1},     // <iterativeStmt>===> TK_WHILE TK_OP <booleanExpression> TK_CL <stmt><otherStmts>TK_ENDWHILE
        {26, 86, 84, 38, 85, 87, 23, 15, 39, -1}, // <conditionalStmt>===> TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt><otherStmts> <factorConditionalStmt>
        {39, 99, 23, 15, 88, -1},                 // <factorConditionalStmt>===> TK_ELSE <stmt><otherStmts> TK_ENDIF | TK_ENDIF
        {39, 88, -1},                             // <factorConditionalStmt>===> TK_ELSE <stmt><otherStmts> TK_ENDIF | TK_ENDIF
        {27, 89, 84, 40, 85, 80, -1},             // <ioStmt>===>TK_READ TK_OP <var> TK_CL TK_SEM
        {27, 90, 84, 40, 85, 80, -1},             // <ioStmt>===>TK_WRITE TK_OP <var> TK_CL TK_SEM
        {30, 41, 42, -1},                         // <arithmeticExpression>===> <arith_term><arithmatic_recursion>
        {42, 43, 41, 42, -1},                     // <arithmatic_recursion>===>  <operator> <arith_term> <arithmatic_recursion>
        {42, 110, -1},                            // <arithmatic_recursion>===>  eps
        {41, 44, 45, -1},                         // <arith_term> ===> <factor> <term_recursion>
        {45, 46, 44, 45, -1},                     // <term_recursion>===> <operator_second> <factor> <term_recursion>
        {45, 110, -1},                            // <term_recursion>===> eps
        {44, 84, 30, 85, -1},                     // <factor> ====>TK_OP <arithmeticExpression> TK_CL
        {44, 40, -1},                             // <factor> ====><var>
        {43, 92, -1},                             // <operator> ===> TK_PLUS | TK_MINUS
        {43, 93, -1},                             // <operator> ===> TK_PLUS | TK_MINUS
        {46, 94, -1},                             // <operator_second> ===> TK_MUL  | TK_DIV
        {46, 95, -1},                             // <operator_second> ===> TK_MUL  | TK_DIV
        {38, 84, 38, 85, 48, 84, 38, 85, -1},     // <booleanExpression>===>TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL
        {38, 40, 49, 40, -1},                     // <booleanExpression>===> <var> <relationalOp> <var>
        {38, 102, 84, 38, 85, -1},                // <booleanExpression>===> TK_NOT TK_OP <booleanExpression> TK_CL
        {40, 56, -1},                             // <var>===> TK_NUM | TK_RNUM | <singleOrRecId>
        {40, 57, -1},                             // <var>===> TK_NUM | TK_RNUM | <singleOrRecId>
        {40, 29, -1},                             // <var>===> TK_NUM | TK_RNUM | <singleOrRecId>
        {48, 100, -1},                            // <logicalOp>===>TK_AND | TK_OR
        {48, 101, -1},                            // <logicalOp>===>TK_AND | TK_OR
        {49, 103, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 104, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 105, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 106, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 107, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {49, 108, -1},                            // <relationalOp>===> TK_LT | TK_LE | TK_EQ |TK_GT | TK_GE | TK_NE
        {16, 91, 50, 80, -1},                     // <returnStmt>===>TK_RETURN <optionalReturn> TK_SEM
        {50, 73, 37, 74, -1},                     // <optionalReturn>===>TK_SQL <idList> TK_SQR
        {50, 110, -1},                            // <optionalReturn>===> eps
        {37, 55, 51, -1},                         // <idList>===> TK_ID <more_ids>
        {51, 79, 37, -1},                         // <more_ids>===> TK_COMMA <idList>
        {51, 110, -1},                            // <more_ids>===> eps
        {18, 66, 12, 59, 67, 59, -1},             // <definetypestmt>===>TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID
        {12, 97, -1},                             // <A>==>TK_RECORD
        {12, 64, -1}                              // <A>==>TK_UNION
    };
    grammar G = (grammar)malloc(sizeof(grammar));
    G->Grammar = (int **)malloc(GRAMMAR_SIZE * sizeof(int *));

    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        G->Grammar[i] = (int *)malloc(10 * sizeof(int));
    }
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            G->Grammar[i][j] = temp[i][j];
            // printf("%d ",temp[i][j]);
            if (temp[i][j] == -1)
                break;
        }
    }
    return G;
}

// Initialize the parser table using first and follow sets
table initialize_table(FirstAndFollow F)
{
    table T = (table)malloc(sizeof(table));
    T->parserTable = (int **)malloc(NONTERMINALS * sizeof(int *));
    // T->parserTable = (int ***)malloc(NONTERMINALS * sizeof(int **));
    for (int i = 0; i < NONTERMINALS; i++)
    {
        T->parserTable[i] = (int *)malloc((TERMINALS + 1) * sizeof(int));
        // T->parserTable[i] = (int **)malloc((TERMINALS + 1) * sizeof(int *));
    }
    for (int i = 0; i < NONTERMINALS; i++)
    {
        for (int j = 0; j <= TERMINALS; j++)
        {
            T->parserTable[i][j] = -1;
            // T->parserTable[i][j] = NULL;
        }
    }
    for (int i = 0; i < NONTERMINALS; i++)
    {
        for (int j = 0; F->follow[i + 1][j] != 0; j++)
        {
            int x = F->follow[i + 1][j];
            if (x == -1)
                T->parserTable[i][0] = SYN;
            // *(T->parserTable[i][0]) = SYN;
            else
                T->parserTable[i][x - NONTERMINALS] = SYN;
            // *(T->parserTable[i][x - NONTERMINALS]) = SYN;
        }
    }
    return T;
}

// Frees the memory allocated to the parse tree
void freeParseTree(parseTree root)
{
    int sz = root->sizeofChild;
    for (int i = 0; i < sz; i++)
    {
        // printf("%d\n",root->current);
        freeParseTree(root->child[i]);
    }
    free(root);
}

// Frees the memory allocated to the first and follow sets
void freeFirstandFollow(FirstAndFollow F)
{
    for (int i = 0; i < TERMS_SIZE; i++)
    {
        free(F->first[i]);
    }
    free(F->first);
    for (int i = 0; i <= NONTERMINALS; i++)
    {
        free(F->follow[i]);
    }
    free(F->follow);
    free(F);
}

// Frees the memory allocated to the parser table
void freeTable(table T)
{
    for (int i = 0; i < NONTERMINALS; i++)
    {
        free(T->parserTable[i]);
    }
    free(T->parserTable);
    free(T);
}

// Frees the memory allocated to the grammar
void freeGrammar(grammar G)
{
    for (int i = 0; i < GRAMMAR_SIZE; i++)
    {
        free(G->Grammar[i]);
    }
    free(G->Grammar);
    free(G);
}

void parser(char *testcaseFile, char *outputFile)
{
    G = initialize_grammar();
    // print_grammar(G);
    // printf("\n");

    FirstAndFollow F = ComputeFirstAndFollowSets(G);
    // print_first_set(F, G);
    // printf("\n");
    // print_follow_set(F, G);
    // printf("\n");

    table T = initialize_table(F);
    createParseTable(F, T);
    // printTable(T);
    // printf("\n");

    // char dummy[5] = "----\0";

    parseTree root;
    root = parseInputSourceCode(testcaseFile, T);
    printParseTree(root, outputFile);

    freeParseTree(root);
    freeFirstandFollow(F);
    freeTable(T);
    freeGrammar(G);
}