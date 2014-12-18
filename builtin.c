/*
 * ============================================================================================
 * Program Name : builtin.c
 * Author :  Hongyi Su (Will)   hsu4@stevens.edu
 * Date:     12/14/2014
 * Description: implement the builtin command
 *
 *
 * =============================================================================================
*/
#include "sish.h"

int builtin_command( char *command, char **parameters, int count, struct parse_info * info)
{
	extern struct passwd *pwd;
	if(strcmp( command, "exit") == 0)
	{
		if(xflag == 1)
			fprintf(stdout, "+ exit\n");
		exit(0);
		return 1;
	}
	else if( strcmp( command, "echo") == 0)
	{
		int pipe_fd[2], ret = 0,out_fd, i = 0, quto = 0;
		int j = 1, signal = 0;
		pid_t ChdPid = 0,ChdPid2 = 0;
		char O_buffer[MAXLINE] = {"\0"}, O_name[Max_name] = {"\0"};
		char *tmp_Output = NULL ;
		if((ChdPid = getpid()) < 0)
		{
			print_err("Error in getpid()\n");
		}
		if(info->flag & IS_PIPED)
		{
			i = 1;
			while(parameters[i] != NULL)
			{
				int length = 0;
				length = strlen(parameters[i]);
				if (parameters[i][0] == '\"')
				{
					quto = 1;
					j = 1;
					while(j<length)
					{
						if( parameters[i][j++] == '$')
						{
							signal = 1;
							break;
						}
					}
					if (signal == 1)
					{
						if(parameters[i][length-1] == '\"')
							memcpy(O_name, parameters[i]+2, strlen(parameters[i])-3);
						else
							memcpy(O_name, parameters[i]+2, strlen(parameters[i])-2);
						if((tmp_Output = get_env( O_name)) != NULL)
							strncat(O_buffer,tmp_Output, strlen(tmp_Output));
						else if(strncmp(O_name,"?",1) == 0)
						{
							char Error[10]={"\0"};
							snprintf(Error,10,"%d",status);
							strncat(O_buffer, Error,strlen(Error));
						}
						else if(strncmp(O_name,"$",1) == 0)
						{
							char C_pid[10] = {"\0"};
							ChdPid = getpid();
							snprintf(C_pid, 10, "%d",ChdPid);
							strncat(O_buffer,C_pid, strlen(C_pid));
						}
						memset(O_name, 0, Max_name);
						strncat(O_buffer," \0", 2);
						if( parameters[i][length-1] == '\"')
							quto = 0;
						i++;
						signal = 0;
						continue;		
					}
					if( parameters[i][length-1] == '\"')
					{
						quto = 0;
						strncat(O_buffer, parameters[i]+1,length-2);
					}
					else
						strncat(O_buffer, parameters[i]+1,length-1);
					strncat(O_buffer," \0", 2);	
					i++;
					continue;
				
				}
				if(parameters[i][0] == '$')
				{
					
					if(parameters[i][length-1] == '\"')	
						memcpy(O_name, parameters[i]+1, strlen(parameters[i])-1);
					else
						memcpy(O_name, parameters[i]+1, strlen(parameters[i]));
					if((tmp_Output = get_env( O_name)) != NULL)
						strncat(O_buffer,tmp_Output, strlen(tmp_Output));
					else if(strncmp(O_name,"?",1) == 0)
					{
						char Error[10]={"\0"};
						snprintf(Error,10,"%d",status);
						strncat(O_buffer, Error,strlen(Error));
					}
					else if(strncmp(O_name,"$",1) == 0)
					{
						char C_pid[10] = {"\0"};
						ChdPid = getpid();
						snprintf(C_pid, 10, "%d",ChdPid);
						strncat(O_buffer,C_pid, strlen(C_pid));
					}
					memset(O_name, 0, Max_name);
					strncat(O_buffer," \0", 2);
					if( parameters[i][length-1] == '\"')
						quto = 0;
					i++;
					continue;												
				}
				
				if( quto == 1 && parameters[i][length-1] == '\"')
				{
					strncat(O_buffer, parameters[i],length-1);
					quto = 0;
				}
				else
					strncat(O_buffer, parameters[i],length);
				strncat(O_buffer," \0", 2);
				i++;
			}				
			if(pipe(pipe_fd)<0)
				print_err("sish error: pipe failed.\n");
			if((ChdPid2 = fork()) == 0)
			{
				
				close(pipe_fd[1]);
				close(fileno(stdin));
				ret = dup2(pipe_fd[0], fileno(stdin));
				if(ret == -1)
					print_err("dup2 error: dup2 failed.\n");
				close(pipe_fd[0]);
				ret = execvp(info->command2, info->parameters2);
				if(ret == -1)
				{
					perror("execvp:");
				}	
				exit(errno);	
			}
			else
			{
				close(pipe_fd[0]);
				write(pipe_fd[1], O_buffer,strlen(O_buffer));
				close(pipe_fd[1]);
				waitpid(ChdPid2, &status, 0);
			}		
		}
		else if(info->flag & BACKGROUND)
		{
			int i = 0;
			for( i = 0; i<MAXPIDTABLE;i++)
				if(BPTable[i] == 0)
				{
					BPTable[i] = ChdPid;
					break;
				}
			if( i == MAXPIDTABLE)
				perror("Too much processes in background\nThere will be zombine process");
		}
		else
		{
			i = 1;
			while(parameters[i]!= NULL)
			{
				int length = 0;
				length = strlen(parameters[i]);
				if (parameters[i][0] == '\"')
				{
					quto = 1;
					j = 1;
					while(j<length)
					{
						if( parameters[i][j++] == '$')
						{
							signal = 1;
							break;
						}
					}
					if (signal == 1)
					{
						if(parameters[i][length-1] == '\"')
							memcpy(O_name, parameters[i]+2, strlen(parameters[i])-3);
						else
							memcpy(O_name, parameters[i]+2, strlen(parameters[i])-2);
						if((tmp_Output = get_env( O_name)) != NULL)
							strncat(O_buffer,tmp_Output, strlen(tmp_Output));
						else if(strncmp(O_name,"?",1) == 0)
						{
							char Error[10]={"\0"};
							snprintf(Error,10,"%d",status);
							strncat(O_buffer, Error,strlen(Error));
						}
						else if(strncmp(O_name,"$",1) == 0)
						{
							char C_pid[10] = {"\0"};
							ChdPid = getpid();
							snprintf(C_pid, 10, "%d",ChdPid);
							strncat(O_buffer,C_pid, strlen(C_pid));
						}
						memset(O_name, 0, Max_name);
						strncat(O_buffer," \0", 2);
						if( parameters[i][length-1] == '\"')
							quto = 0;
						i++;
						continue;		
					}
					if( parameters[i][length-1] == '\"')
					{
						quto = 0;
						strncat(O_buffer, parameters[i]+1,length-2);
					}
					else
						strncat(O_buffer, parameters[i]+1,length-1);
					strncat(O_buffer," \0", 2);
					signal = 0;
					i++;
					continue;
						
				}
				if(parameters[i][0] == '$')
				{
					
					if(parameters[i][length-1] == '\"')	
						memcpy(O_name, parameters[i]+1, strlen(parameters[i])-1);
					else
						memcpy(O_name, parameters[i]+1, strlen(parameters[i]));
					if((tmp_Output = get_env( O_name)) != NULL)
						strncat(O_buffer,tmp_Output, strlen(tmp_Output));
					else if(strncmp(O_name,"?",1) == 0)
					{
						char Error[10]={"\0"};
						snprintf(Error,10,"%d",status);
						strncat(O_buffer, Error,strlen(Error));
					}
					else if(strncmp(O_name,"$",1) == 0)
					{
						char C_pid[10] = {"\0"};
						ChdPid = getpid();
						snprintf(C_pid, 10, "%d",ChdPid);
						strncat(O_buffer,C_pid, strlen(C_pid));
					}
					memset(O_name, 0, Max_name);
					strncat(O_buffer," \0", 2);
					if( parameters[i][length-1] == '\"')
						quto = 0;
					i++;
					continue;												
				}
				if( quto == 1 && parameters[i][length-1] == '\"')
				{
					strncat(O_buffer, parameters[i],length-1);
					quto = 0;
				}
				else
					strncat(O_buffer, parameters[i],length);
				strncat(O_buffer," \0", 2);
				i++;
				
			}
			if(!(info->flag & OUT_REDIRECT) && !(info->flag & OUT_REDIRECT_APPEND))
				fprintf( stdout, "%s\n", O_buffer);
			else
			{
				if(info->flag & OUT_REDIRECT)
				{
					out_fd = open(info->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
					if(out_fd == -1)
						print_err("open error: open failed.\n");
				}
				else
				{
					out_fd = open(info->out_file, O_WRONLY | O_APPEND, 0666);
					if(out_fd == -1)
						print_err("open error: open failed.\n");
				}
				write(out_fd, O_buffer,strlen(O_buffer));
				close(out_fd);
			}
		}
		return 1;
	}
	else if( strcmp( command, "cd") == 0)
	{
		char *cd_path = NULL;
		if( count == 1)
		{
			cd_path = (char *)malloc((strlen(pwd->pw_dir)+1)*sizeof(char));
			if (cd_path == NULL)
			{
				fprintf( stderr, "Error in malloc().\n");
				exit(1);
			}
			memset(cd_path,0, strlen(pwd->pw_dir)+1);
			memcpy( cd_path, pwd->pw_dir, strlen(pwd->pw_dir));

		}
		else
		{
			if( parameters[1][0] == '~')
			{
				cd_path = (char *)malloc((strlen(pwd->pw_dir)+ strlen(parameters[1]))*sizeof(char));
				if (cd_path == NULL)
				{
					fprintf( stderr, "Error in malloc().\n");
					exit(1);
				}
				memset(cd_path, 0, strlen(pwd->pw_dir)+ strlen(parameters[1]));
				memcpy( cd_path, pwd->pw_dir, strlen(pwd->pw_dir));
				memcpy( cd_path+strlen(pwd->pw_dir), parameters[1]+1, strlen(parameters[1]));
			
			}
			else
			{
				cd_path = (char *)malloc((strlen(parameters[1])+1)*sizeof(char));
				memset(cd_path,0,strlen(parameters[1])+1);
				if( cd_path == NULL)
				{
					fprintf( stderr, "Error in malloc().\n");
					exit(1);
				}
				memcpy( cd_path, parameters[1], strlen(parameters[1])+1);
			}
		}
		if(xflag == 1)
			fprintf(stdout, "+ cd\n");
		if(chdir(cd_path)!= 0)
			fprintf( stderr, "+ sish : cd: %s:%s\n", cd_path, strerror(errno));
		free(cd_path);
		return 1;
	}
	return 0;
	
}