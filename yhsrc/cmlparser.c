#include <string.h> /* for strdup */
#include <assert.h> /* for assertion */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for getopt */

#define MAXFILEARG 100
#define MAXTOKENS 1000
#define TOKENDELM " \t\n"

/* ---------------- */

typedef struct {
    int argc;
    char **argv;
    int stdout_redirect;
    int stdin_redirect;
    char *outfile;
    char *infile;
} Program;

Program *Program_create(int argc, char **argv, int stdout_redirect, int stdin_redirect, char *outfile, char *infile);
void Program_destroy(Program *p);
void Program_print(Program *p);

Program *Program_create(int argc, char **argv, int stdout_redirect, int stdin_redirect, char *outfile, char *infile)
{
    Program *p = malloc(sizeof(Program));
    assert(p != NULL);

    p -> argc = argc;
    p -> stdout_redirect = stdout_redirect;
    p -> stdin_redirect = stdin_redirect;
    p -> outfile = strdup(outfile);
    p -> infile = strdup(infile);
    p -> argv = malloc(argc * sizeof(char *));
    for (int i = 0; i < argc; i++) {
	(p -> argv)[i] = strdup(argv[i]);
    }

    return p;
}

void Program_destroy(Program *p)
{
    assert(p != NULL);

    free(p -> outfile);
    free(p -> infile);
    for (int i = 0; i < p -> argc; i++) {
	assert((p -> argv)[i] != NULL);
	free((p -> argv)[i]);
    }
    free(p -> argv);
    free(p);
}

void Program_print(Program *p)
{
    printf("argc: %d\n", p -> argc);
    printf("stdout_redirect: %d\n", p -> stdout_redirect);
    printf("stdin_redirect: %d\n", p -> stdin_redirect);
    printf("outfile: %s\n", p -> outfile);
    printf("infile: %s\n", p -> infile);
    for (int i = 0; i < p -> argc; i++) {
	printf("argv[%d]: %s\n", i, (p -> argv)[i]);
    }    
}


/* ----------------- */

int dflag = 0;
int xflag = 0;
int fflag = 0;
int debugLevel = 0;
char *batchfile;
char *filearg[MAXFILEARG];

/* parse the command line arguments when executing sish */
void parsecml(int argc, char *argv[])
{
    extern char *optarg;
    extern int optind;
    int c, err = 0;
    char *usage = "usage: sish [-x] [-d <level>] [-f file [arg] ...]";

    while ((c = getopt(argc, argv, "xd:f:")) != -1) {
	switch (c) {
	case 'x':
	    xflag = 1;
	    break;
	case 'd':
	    if (dflag == 1) {
		fprintf(stderr, "Error: -d has been set already\n");
	    }
	    dflag = 1;
	    debugLevel = atoi(optarg);
	    break;
	case 'f':
	    if (fflag == 1) {
		fprintf(stderr, "Error: -f has been set already\n");
	    }
	    fflag = 1;
	    batchfile = optarg;
	    break;
	case '?':
	    err = 1;
	    break;
	default:
	    break;
	}
	if (err) {
	    fprintf(stderr, "%s\n", usage);
	    exit(1);
	}	
    }

    if (optind < argc) { /* arguments assigned to shell variables */
	for (int i = 0; optind < argc; i++) {
	    filearg[i] = argv[optind];
	    optind++;
	}
    }
}

/* tokenize the command line arguments,
it takes the input string and the address
of the pointer of an array of string tokens */
int tokcml(char *input, char **tokens[])
{
    int maxsize = MAXTOKENS, cursize = 0;
    char *inputcpy = strdup(input);    
    char *token;

    printf("!");
    if ((*tokens =  malloc(maxsize * sizeof(char*))) == NULL) {
	fprintf(stderr, "error: memory allocation failed\n");
    }
    token = strtok(inputcpy, TOKENDELM);

    while (token != NULL) {
	(*tokens)[cursize] = strdup(token);
	cursize++;

	if (cursize >= maxsize) {
	    maxsize *= 2;
	    if ((*tokens = realloc(*tokens, maxsize * sizeof(char *))) == NULL) {
		fprintf(stderr, "error: memory allocation failed\n");
	    }
	}
	
	token = strtok(NULL, TOKENDELM);
    }

    (*tokens)[cursize] = NULL;
    free(inputcpy);
    return 0;
}

/* free the tokens, need to be called after done with tokens */
int token_destroy(char *tokens[])
{
    for (int i = 0; tokens[i] != NULL; i++) {
	assert(tokens[i] != NULL);
	free(tokens[i]);
    }
    assert(tokens != NULL);
    free(tokens);
    return 0;
}

/* print the error message if debug level is larger than 0 */
void printerr(int debugLevel, char *errmsg) {
    if (debugLevel > 0) {
	fprintf(stderr, "%s\n", errmsg);
    }
}

/* go through tokens and store the 
arguments for each program, this function
should be called after tokcml */
int getArgs()
{
    /* TODO */
    return 0;
}

/* return the number of pipes */
int ispiped()
{
    /* TODO */
    return 0;
}

/* the only parser needed in the shell loop.
It needs to be called at the begining of the
loop. It construct all the arguments for programs*/
int parse_input_line(void)
{
    /* TODO */
    char *line;
    size_t linecap = 0;
    getline(&line, &linecap, stdin);
    return 0;
}

/* unit testing */
int main(int argc, char *argv[])
{
    /* parsecml test */
    parsecml(argc - 1, argv + 1);
    
    printf("dflag: %d\n", dflag);
    printf("xflag: %d\n", xflag);
    printf("fflag: %d\n", fflag);
    printf("debugLevel: %d\n", debugLevel);
    printf("filename: %s\n", batchfile);
    for (int i = 0; filearg[i] != NULL; i++) {
	printf("filearg[%d]: %s\n", i, filearg[i]);
    }

    /* tokcml test */
    char *input = "echo hello | wc";
    char **tokens;

    tokcml(input, &tokens);
    for (int i = 0; tokens[i] != NULL; i++) {
	printf("tokens[%d]: %s\n", i, tokens[i]);
    }
    token_destroy(tokens);

    /* printerr test */
    char *errorstr = "This is an error";
    printerr(debugLevel, errorstr);

    /* program struct test */
    char *test_argv[3];
    test_argv[0] = "myprog";
    test_argv[1] = "arg1";
    test_argv[2] = "arg2";
    Program *p = Program_create(3, test_argv, 1, 1, "F2", "F1");
    Program_print(p);
    Program_destroy(p);
}
