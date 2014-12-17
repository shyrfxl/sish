/*
 * ============================================================================================
 * Program Name : parse.c
 * Author :  Hongyi Su (Will)   hsu4@stevens.edu
 * Date:     12/15/2014
 * Description: parse the command and parameters from read_command.c
 *
 *
 * =============================================================================================
*/
#include "sish.h"
//----------------------------------------------------------------------------------------------
int parse_info_init(struct parse_info *info)
{
	info->flag = 0;
	info->in_file = NULL;
	info->out_file = NULL;
	info->command2 = NULL;
	info ->parameters2 = NULL;
	return 0;
}
//----------------------------------------------------------------------------------------------
int parsing( char **parameters, int arg_num, struct parse_info *info)
{
	int i = 0;
	parse_info_init(info);
	if(strcmp(parameters[arg_num-1],"&") == 0)
	{
		fprintf( stdout,"&&\n");
		info->flag |= BACKGROUND;
		parameters[arg_num-1] = NULL;
		arg_num--;
	}
	for( i = 0;i<arg_num;)
	{
		if(strcmp(parameters[i], "<<")== 0 || strcmp(parameters[i], "<") == 0)
		{
			info->flag |= IN_REDIRECT;
			info->in_file = parameters[i+1];
			parameters[i] = NULL;
			i+=2;
		}
		else if( strcmp(parameters[i], ">") == 0)
		{
			info->flag |= OUT_REDIRECT;
			info->out_file = parameters[i+1];
			parameters[i] = NULL;
			i+= 2;
		}
		else if( strcmp(parameters[i], ">>") == 0)
		{
			info->flag |= OUT_REDIRECT_APPEND;
			info->out_file = parameters[i+1];
			parameters[i] = NULL;
			i+= 2;
			fprintf(stdout,"11\n");
		}
		else if( strcmp(parameters[i], "|") == 0)
		{
			char * pch;
			info->flag |= IS_PIPED;
			parameters[i] = NULL;
			info->command2 = parameters[i+1];
			info->parameters2 = &parameters[i+1];
			for( pch = info->parameters2[0]+strlen(info->parameters2[0]);\
			pch!=&(info->parameters2[0][0]) && *pch != '/'; pch --);
			if(*pch == '/')
				pch++;
			info->parameters2[0] = pch;
			break;
			
		}
		else
			i++;
	
	}
/*	
	fprintf(stdout,"\nbackground:%s\n",info->flag&BACKGROUND?"yes":"no");
	fprintf(stdout,"in redirect:");
	info->flag&IN_REDIRECT?fprintf(stdout,"yes,file:%s\n",info->in_file):fprintf(stdout,"no\n");
	fprintf(stdout, "out redirect:");
	info->flag&OUT_REDIRECT?fprintf(stdout,"yes,file:%s\n",info->out_file):fprintf(stdout,"no\n");
	fprintf(stdout, "pipe:");
	info->flag&IS_PIPED?fprintf(stdout,"yes,command:%s %s %s\n",info->command2,info->parameters2[0],info->parameters2[1]):fprintf(stdout,"no\n");
*/	
	return 1;
}





























