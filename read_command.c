/*
 * ============================================================================================
 * Program Name : read_command.c
 * Author :  Hongyi Su (Will)   hsu4@stevens.edu
 * Date:     12/15/2014
 * Description: read one line from user input and separate into to right positions 
 *              
 *
 * =============================================================================================
*/

#include "sish.h"
#include "stdio.h"
#include "readline/readline.h"
#include "readline/history.h"
#define MAX_arg 20

int read_command( char **command, char **parameters, char *prompt)
{
	char *tmpbuffer = buffer;
	buffer = readline( prompt);
/*
	if( feof(stdin) == 0)
	{
		printf("11\n");
		exit(0);
	}
*/
//	fprintf(stdout,"%s\n",buffer);
	if(buffer[0] == '\0')
		return -1;
	char *p_start = NULL, *p_end = NULL;
	int count = 0;
	int isFinished = 0;
	p_start = p_end = buffer;
	while( !isFinished)
	{
		while((*p_end == ' ' && *p_start == ' ') || (*p_end == '\t' && *p_start == '\t'))
		{
			p_start++;
			p_end++;
		}
		if(*p_end == '\0' || *p_end == '\n')
		{
			if(count == 0)
				return -1;
			break;
		}
		while(*p_end != ' ' && *p_end != '\n' && *p_end != '\0')
			p_end++;

		if(*p_end == '\0' || *p_end == '\n')
		{
			*p_end = '\0';
			isFinished = 1;
		}
		else
		{
			*p_end = '\0';
		}

		if( count == 0)
		{
			char *tmp = p_end;
			//memcpy(*command,p_start,strlen(p_start));
			*command = p_start;
			while( tmp != p_start && *tmp != '/')
				tmp--;
			if(*tmp == '/')
				tmp++;
			//memcpy(parameters[0],tmp,strlen(tmp));
			parameters[0] = tmp;
			count +=2;
			 
		}
		else if( count <= MAX_arg)
		{
			//memcpy(parameters[count-1],p_start,strlen(p_start));
			parameters[count-1] = p_start;
			count++;
		}
		else
		{
			break;
		}
		p_end++;
		p_start = p_end;
	}
	parameters[count-1] = NULL;
	fprintf(stdout,"count : %d\n",count);
	fprintf(stdout, "Input analysis:\n");
	fprintf(stdout, "pathname: %s\ncommand:%s\nparameters:\n", *command, parameters[0]);
	int i;
	for( i=0; i<count-1;i++)
	{
		fprintf(stdout,"%s\n", parameters[i]);
	}
	//free(buffer);
	buffer = tmpbuffer;
	
	return count;


}