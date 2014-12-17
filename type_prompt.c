/*
 * ============================================================================================
 * Program Name : type_prompt.c
 * Author :  Hongyi Su (Will)   hsu4@stevens.edu
 * Date:     12/14/2014
 * Description: prepare for prompt of type
 *
 *
 * =============================================================================================
*/
#include "sish.h"

void type_prompt( char *prompt)
{
	extern struct passwd *pwd;
	char h_name[Max_name] = {"\0"};
	char p_name[MAX_PATH] = {"\0"};
	int length = 0;
	pwd = getpwuid(geteuid());
	
	getcwd(p_name, Max_name);
	if (gethostname(h_name, Max_name) == 0)
		snprintf(prompt, MAXLINE, "[sish] %s@%s:",pwd->pw_name, h_name);
	else
		snprintf(prompt, MAXLINE, "[sish] %s@unknown:", pwd->pw_name);

	length = strlen(prompt);

	if(strlen(p_name) < strlen(pwd->pw_dir) || strncmp(p_name, pwd->pw_dir, strlen(pwd->pw_dir)) != 0)
		snprintf(prompt + length, MAXLINE, "%s", p_name);
	else
		snprintf(prompt + length, MAXLINE, "~%s", p_name+strlen(pwd->pw_dir));
	
	length = strlen(prompt);

	if(geteuid() == 0)
		snprintf( prompt + length, MAXLINE,  "#");
	else
		snprintf( prompt + length, MAXLINE,  "$");
	return;
}

/*
int main()
{
	char *prompt = NULL;
	prompt = (char *)malloc( MAXLINE*sizeof(char));
	type_prompt( prompt);
	fprintf( stdout, "%s\n", prompt);
	return 0;
}
*/