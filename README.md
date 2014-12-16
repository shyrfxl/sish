NAME:
	sish — a simple hell

SYNOPSIS:	
	sish [ -x] [ -c command]

DESCRIPTION:
	sish implements a very simple command-line interpreter or shell. It is suitable to be used interactively or
as a login shell. It only implements a very small subset of what would usually be expected of a Unix shell,
and does explicitly not lend itself as a scripting language.

OPTIONS:
	The following options are supported by sish:
	-c command	Execute the given command.
	-x Enable tracing: Write each command to standard error, preceeded by ’+’.

DETAILS:
	sish allows for the execution of the given commands with a minimal amount of niceties.
When sish is invoked and no commands are passed via the -c flag, it will print a simple prompt to stdout.It then reads commands from the controlling terminal, executing them as one would expect.

CONSIST OF:
	
	-sish.c
		main program
 
	-type_prompt.c
		print out the prompt of sish including path, hostname

	-read_command.c
		read command input, and analyse the command and parameter(s)

	-builtin.c
		support some built-in command, such as exit, echo, and cd

	-parse.c
		analyses user's input line and tell them to sish.

Redirection:
	sish supports the following three input-/output- redirection operators:
	> file Redirect standard output to file. Mutually exclusive with the >> opreator.
	>> file Append standard output to file. Mutually exclusive with the > opreator.
	< file Redirect standard input from file.

Pipeines:
	A pipeline is a sequence of one or more commands separated by the control operator ’|’. The standard output
of all but the last command is connected to the standard input of the next command. The standard output of
the last command is, absent any redirection operators, the controlling terminal.

Background Commands -- &:
	If a command is terminated by the control operator ampersand (&), the shell executes the command asynchronously
-- that is, the shell does not wait for the command to finish before prompting the user for the next
command.

Builtins:
	sish supports the following builtins (which are taking precedence over any non-builtin commands):
	cd [dir] 	Change the current working directory. If dir is not specified, change to the user’s home
directory.
	
	echo [word] 	Print the given word, followed by a ’\n’. The following special values are supported:
	$? The exit status of the last command.
	$$ The current process ID.
	
	exit 		Exit the current shell.

Command Execution:
	If a sequence of words does not begin with a builtin, sish will attempt to execute it as a command, possibly
utilizing the current PATH.

Environment:
	sish 	sets the following environment variables:
	SHELL 	the path of the executable of sish

EXAMPLES:
	The following sequence of commands shows common usage of sish:
	$ sish
	sish$ echo $$
	6465
	sish$ ls
	file1 file2	
	sish$ ls | wc -l
	2
	sish$ echo $?	
	0	
	sish$ find / >/dev/null &
	sish$
	sish$ aed -e -p ’passphrase’ -s ’0a1b2c3d’ <file >file.enc	
	sish$ cmd | sort | uniq -c | sort -n
	121 foo
	304 bar	
	sish$ something
	something: command not found	
	sish$ echo $?
	127
	sish$ rm /etc/passwd
	rm: /etc/passwd: Permission denied
	sish$ echo $?
	1
	sish$ exit
	$ sish -c date
	Sat Nov 29 21:18:07 EST 2014
	$ sish -x
	sish$ ls
	+ ls
	file1 file2
	sish$ ls | wc -l
	+ ls
	+ wc -l
	2
	sish$ cd /tmp
	+ cd /tmp	
	sish$ pwd
	+ pwd
	/tmp
	sish$ cd
	+ cd
	sish$ pwd
	+ pwd
	/home/jschauma
	sish$ exit
	+ exit
	$

EXIT STATUS:
	sish returns the exit status of the last command it executed or a status of 127 if the given command could
not be executed for any reason.

SEE ALSO:
	bash(1), ksh(1), sh(1), execve(2), fork(2)

