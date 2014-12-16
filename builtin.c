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

int builtin_command( char *command, char **parameters, int count)
{
	extern struct passwd *pwd;
	fprintf(stdout,"builtin:%s\n",command);
	if(strcmp( command, "exit") == 0)
	{
		fprintf(stdout,"exit()\n");
		exit(0);
	}
	else if( strcmp( command, "echo") == 0)
	{
		//
		fprintf( stdout, "%s\n", parameters[1]);
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
			fprintf(stdout,"builtin:%s\n",cd_path);

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
		fprintf(stdout,"builtin:%s\n",cd_path);
		if(chdir(cd_path)!= 0)
			fprintf( stderr, "+sish : cd: %s:%s\n", cd_path, strerror(errno));
		free(cd_path);
	}
	return 0;
	
}