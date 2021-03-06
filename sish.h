/*
 * ============================================================================================
 * Program Name : sish.c
 * Author :  Hongyi Su (Will)   hsu4@stevens.edu
 * Date:     12/15/2014
 * Description: head file of whole project 
 *              
 *
 * =============================================================================================
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h> 
#define MAX_PROMPT 1024
#define MAXPIDTABLE 1024
#define MAXLINE 4096 //the length of all args is ARG_MAX
#define MAXARG 20
#define MAX_PATH 2048
#define Max_name 256

struct parse_info;
struct passwd *pwd;
char *buffer;
short cflag;
short xflag;
pid_t BPTable[MAXPIDTABLE];
int status;
void type_prompt(char*);
int read_command(char **,char **,char*);
int builtin_command(char *,char **,int, struct parse_info *);
int parsing(char **,int,struct parse_info *);
void proc(void);
void sig_handler(int);
void unsetReqMetaVar(char *);
void setReqMetaVar(char *, char *);
void print_err(const char *);
char* get_env( char *);

#define STRUCT_PARSE_INFO
#define BACKGROUND 		1
#define IN_REDIRECT 		2
#define OUT_REDIRECT 		4
#define OUT_REDIRECT_APPEND	8
#define IS_PIPED 		16
struct parse_info 
{
    	int flag;
    	char* in_file;
    	char* out_file;
    	char* command2;
 	char** parameters2;
};