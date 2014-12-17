/*
 * ============================================================================================
 * Program Name : sish.c
 * Author :  Hongyi Su (Will)   hsu4@stevens.edu
 * Date:     12/15/2014
 * Description: main function to get options by getopt() and analyse the option to 
 *              run the function.
 *
 * =============================================================================================
*/

#include "sish.h"
char program_n[MAX_PATH] = {"\0"};
char *getprogname();
void proc();
void setprogname(char *r1);
void init_env( char *tmp_buffer);
void run_command(char* command);
//-----------------------------------------------------------------------------
void Usage()
{
	fprintf(stdout, "usage: %s [-x] [-c command](more details by option \'h\')\n",\
	 getprogname());
	exit( EXIT_FAILURE );	
}

//-----------------------------------------------------------------------------
void Usagehelp()
{
	fprintf(stdout,"usage: %s <cdhilp>\n \
	-c command\t Execute the given command.\n\
	-x    \t\t Enable tracing: Write each command to standard error, \
	preceeded by \'+\'\
	-h \t\t Print a short usage summary and exit.\n",getprogname());
	exit( EXIT_SUCCESS);
}
//-----------------------------------------------------------------------------
void setprogname(char * r1)
{
	memset(program_n,0, MAX_PATH);
	memcpy(program_n, r1, strlen(r1)+1);
}

//-----------------------------------------------------------------------------

char *getprogname()
{
	return program_n;
}
//-----------------------------------------------------------------------------
void print_err(const char *msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(EXIT_FAILURE);
}
//-----------------------------------------------------------------------------
void sig_handler(int sig)
{
	pid_t pid;
	int i;
	for( i=0; i<MAXPIDTABLE; i++)
	{
		if(BPTable[i] != 0)
		{
			pid = waitpid(BPTable[i], NULL, WNOHANG);
			if(pid > 0)
			{
				fprintf(stdout,"process %d exited.\n",pid);
				BPTable[i] = 0;
			}
			else if(pid < 0)
			{
				if(errno != ECHILD)
					perror("waitpid error");
			}

		}
	}
	return;
}
//-----------------------------------------------------------------------------
void proc(void)
{	
	status = 0;
	char *command = NULL;
	char **parameters;
	int arg_num = 0;
	char prompt[MAX_PROMPT] = {"\0"};
	struct parse_info info;
	pid_t ChdPid,ChdPid2;
	parameters = (char **)malloc((MAXARG+2)*sizeof(char *));
	if (parameters == NULL)
		print_err("Error in malloc");
	buffer = (char *)malloc(MAXLINE*sizeof(char));
	if (buffer == NULL)
		print_err("Error in malloc");
	if(signal(SIGCHLD,sig_handler) == SIG_ERR)
		perror("signal() error");
	while(1)
	{
		int pipe_fd[2], in_fd, out_fd, ret = 0;
		type_prompt(prompt);
		arg_num = read_command(&command, parameters,prompt);
		//fprintf(stdout,"sish.c:%s, %s\n",command, parameters[0]);
		if( arg_num == -1)
			continue;
		arg_num--;
		parsing(parameters, arg_num, &info);
		if(builtin_command( command, parameters,arg_num,&info))
			continue;
		if(info.flag & IS_PIPED)
		{
			if(pipe(pipe_fd)<0)
				print_err("sish error: pipe failed.\n");
		}
		if((ChdPid = fork())!=0)
		{
			if(info.flag & IS_PIPED)
			{
				if((ChdPid2 = fork()) == 0)
				{
					close(pipe_fd[1]);
					close(fileno(stdin));
					ret = dup2(pipe_fd[0], fileno(stdin));
					if(ret == -1)
						print_err("dup2 error: dup2 failed.\n");
					close(pipe_fd[0]);
					ret = execvp(info.command2, info.parameters2);
					if(ret == -1)
						print_err("execvp error: execvp failed.\n");
				}
				else
				{
					close(pipe_fd[0]);
					close(pipe_fd[1]);
					if(xflag == 1)
					{
						fprintf(stderr,"+ %s\n",command);
						fprintf(stderr,"+ %s\n",info.command2);
					}
					waitpid(ChdPid2, &status, 0);
				}
			}
			if(info.flag & BACKGROUND)
			{
				fprintf(stdout, "Child pid:%u\n",ChdPid);
				int i = 0;
				for( i = 0; i<MAXPIDTABLE;i++)
					if(BPTable[i] == 0)
					{
						BPTable[i] = ChdPid;
						break;
					}
				if( i == MAXPIDTABLE)
					perror("Too much processes in background\nThere will be zombine process");
				if(xflag == 1)
				{
					fprintf(stderr,"+ %s\n",command);
				}
			}
			else
			{
				if(xflag == 1)
				{
					fprintf(stderr,"+ %s\n",command);
				}
				waitpid( ChdPid, &status, 0);
			}
		}
		else
		{
			if(info.flag & IS_PIPED)
			{
				if(!(info.flag & OUT_REDIRECT) && !(info.flag & OUT_REDIRECT_APPEND))
				{
					close(pipe_fd[0]);
					close(fileno(stdout));
					ret = dup2(pipe_fd[1], fileno(stdout));
					if(ret == -1)
						print_err("dup2 error: dup2 failed.\n");
					close(pipe_fd[1]);
				}
				else
				{
					close(pipe_fd[0]);
					close(pipe_fd[1]);
					if(info.flag & OUT_REDIRECT)
					{
						out_fd = open(info.out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
						if(out_fd == -1)
							print_err("open error: open failed.\n");
					}
					else
					{
						out_fd = open(info.out_file, O_WRONLY | O_APPEND, 0666);
						if(out_fd == -1)
							print_err("open error: open failed.\n");
					}
					close(fileno(stdout));
					ret = dup2(out_fd, fileno(stdout));
					if(ret == -1)
						print_err("dup2 error: dup2 failed.\n");
					close(out_fd);
				}
			}
			else
			{
				if(info.flag & OUT_REDIRECT)
				{
					out_fd = open(info.out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
					if(out_fd == -1)
						print_err("open error: open failed.\n");
					close(fileno(stdout));
					ret = dup2(out_fd, fileno(stdout));
					if(ret == -1)
						print_err("dup2 error: dup2 failed.\n");
					close(out_fd);
				}
				if(info.flag & OUT_REDIRECT_APPEND)
				{
					out_fd = open(info.out_file, O_WRONLY | O_APPEND, 0666);
					if(out_fd == -1)
						print_err("open error: open failed.\n");
					close(fileno(stdout));
					ret = dup2(out_fd, fileno(stdout));
					if(ret == -1)
						print_err("dup2 error: dup2 failed.\n");
					close(out_fd);

				}
			}
			if( info.flag & IN_REDIRECT)
			{
				in_fd = open(info.in_file, O_RDONLY, 0666);
				if(in_fd == -1)
					print_err("open error: open failed.\n");
				close(fileno(stdin));
				ret = dup2(in_fd, fileno(stdin));
				if(ret == -1)
					print_err("dup2 error: dup2 failed.\n");
				close(in_fd);
			}
			ret = execvp(command,parameters);
			if(ret == -1)
				print_err("execvp error: execvp failed.\n");
		}
		
	}
	free(parameters);
	free(buffer);
}

//-----------------------------------------------------------------------------
void 
setReqMetaVar(char *var, char *val)
{
	if (val == NULL){
		return;
	}
	if (setenv(var, val, 1) == -1) {
		print_err("Error in set environment value");
	}
}
//-----------------------------------------------------------------------------
void 
unsetReqMetaVar(char *var)
{
	if (unsetenv(var) == -1) {
		print_err("Error in unset environment value");
	}
}
//-----------------------------------------------------------------------------
void init_env( char *tmp_buffer)
{
	unsetReqMetaVar("SHELL");
	setReqMetaVar("SHELL",tmp_buffer);
}
//-----------------------------------------------------------------------------
char* get_env( char *var)
{
	char* tmp = NULL;
	if((tmp = getenv(var)))
		return tmp;
	return NULL;
}
//-----------------------------------------------------------------------------
void *Calloc(size_t n, size_t size)
{
	void *ptr = NULL;
	if(( ptr = calloc(n,size))== NULL)
		print_err("calloc error\n");
	return ptr;
			
}
//-----------------------------------------------------------------------------
void run_command(char* command)
{
	char Buf[MAX_PATH] = {"\0"};
	FILE *rfd = NULL;
	if((rfd = popen(command, "r")) == NULL) 
	{
		print_err("Error in popen()\n");
	}
	while(fgets(Buf, MAX_PATH, rfd)!=NULL)
	 {
		fprintf(stdout,"%s",Buf);
	}
	pclose(rfd);
	exit(0);
}
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int ch = 0, i = 0;
	char *tmp_buffer = NULL;
	char *one_tcommand = NULL;
	tmp_buffer = (char *)malloc(MAX_PATH*sizeof(char));
	if( tmp_buffer == NULL)
		print_err("Error in malloc\n");
	memset( tmp_buffer, 0, MAX_PATH);
	for( i = 0; i < MAXPIDTABLE; i++)
		BPTable[i] = 0;
	setprogname( argv[0]);
	while((ch = getopt(argc, argv ,"c:xh"))!= -1)
	{
		switch(ch)
		{
			case 'c':
				cflag = 1;
				one_tcommand = (char *)Calloc(strlen(optarg)+1,sizeof(char));
				memcpy(one_tcommand, optarg, strlen(optarg));
				run_command(one_tcommand);
				break;	
			case 'x':
				xflag = 1;
				break;
			case 'h':
				Usagehelp();
				break;
			case '?':
				fprintf(stderr,"Unkown option: %c\n",(char)optopt);
				Usage();
				break;
			default:
				break;
		}
	}
	if(program_n[0] == '.' && program_n[1] == '/')
	{
		if((getcwd(tmp_buffer,MAX_PATH)) == NULL)
		{
			perror("Error in getcwd:");
			exit(EXIT_FAILURE);
		}
		strncat(tmp_buffer, program_n+1, MAX_PATH);
	}
	else
	{
		memcpy(tmp_buffer, program_n, strlen(program_n));
	}
	init_env( tmp_buffer);
/*
	tmp_buffer = get_env( "SHELL");
	fprintf(stdout, "%s\n",tmp_buffer);
*/
	proc();
	return 0;
}
