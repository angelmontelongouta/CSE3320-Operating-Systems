// The MIT License (MIT)
//
// Copyright (c) 2016 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/*
	Name: Angel Montelongo
	ID:	  1001665238
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports four arguments

int main()
{
	char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

	int pidhistory[15];	// Will use this to keep track of pid history everytime
											//fork is called
	char commandhistory[15][MAX_COMMAND_SIZE];
											// Will us this to keep track of command history after
											//every while loop
	int pidindex = -1;	// Used to navigate through the pidhistory array; will be
											//incremented every loop there is a need pid
	int commandindex = -1; // Used to navigate through the command history; will
											// be incremented every time in the loop
	pid_t pid;					// initalizing pid for the fork function.

	while( 1 )
	{
	// Print out the msh prompt
	printf ("msh> ");

	// Read the command from the commandline.  The
	// maximum command that will be read is MAX_COMMAND_SIZE
	// This while command will wait here until the user
	// inputs something since fgets returns NULL when there
	// is no input
	while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

	/* Parse input */
	char *token[MAX_NUM_ARGUMENTS];

	int   token_count = 0;

	// Pointer to point to the token
	// parsed by strsep
	char *argument_ptr;

	//I have a combined of 14 if statements. Their job is to see if the user has
	//inputed !n as a command. It will then check first of their is a command
	//in command_history[n]. If there is no command, loop starts over again.
	//if it find a command, then it will run that command.
	if((strcmp(command_string,"!0\n")) == 0)
	{
		if(0 <= commandindex)
		{
			strcpy(command_string,commandhistory[0]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!1\n")) == 0)
	{
		if(1 <= commandindex)
		{
			strcpy(command_string,commandhistory[1]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!2\n")) == 0)
	{
		if(2 <= commandindex)
		{
			strcpy(command_string,commandhistory[2]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!3\n")) == 0)
	{
		if(3 <= commandindex)
		{
			strcpy(command_string,commandhistory[3]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!4\n")) == 0)
	{
		if(4 <= commandindex)
		{
			strcpy(command_string,commandhistory[4]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!5\n")) == 0)
	{
		if(5 <= commandindex)
		{
			strcpy(command_string,commandhistory[5]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!6\n")) == 0)
	{
		if(6 <= commandindex)
		{
			strcpy(command_string,commandhistory[6]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!7\n")) == 0)
	{
		if(7 <= commandindex)
		{
			strcpy(command_string,commandhistory[7]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!8\n")) == 0)
	{
		if(8 <= commandindex)
		{
			strcpy(command_string,commandhistory[8]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!9\n")) == 0)
	{
		if(9 <= commandindex)
		{
			strcpy(command_string,commandhistory[9]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!10\n")) == 0)
	{
		if(10 <= commandindex)
		{
			strcpy(command_string,commandhistory[10]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!11\n")) == 0)
	{
		if(11 <= commandindex)
		{
			strcpy(command_string,commandhistory[11]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!12\n")) == 0)
	{
		if(12 <= commandindex)
		{
			strcpy(command_string,commandhistory[12]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!13\n")) == 0)
	{
		if(13 <= commandindex)
		{
			strcpy(command_string,commandhistory[13]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}
	if((strcmp(command_string,"!14\n")) == 0)
	{
		if(14 <= commandindex)
		{
			strcpy(command_string,commandhistory[14]);
		}
		else
		{
			printf("Command not in history\n");
			continue;
		}
	}

	//This strcpy is used to store the command in the commandhistory. commandindex
	//will be incrementing pre. I set it here so if the previous if statements
	//with !n, commandstring can be updated properly.
	strcpy(commandhistory[++commandindex], command_string);

	char *working_string  = strdup( command_string );

	// we are going to move the working_string pointer so
	// keep track of its original value so we can deallocate
	// the correct amount at the end
	char *head_ptr = working_string;

	// Tokenize the input strings with whitespace used as the delimiter
	while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) &&
	          (token_count<MAX_NUM_ARGUMENTS))
	{
	  token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
	  if( strlen( token[token_count] ) == 0 )
	  {
	    token[token_count] = NULL;
	  }
	    token_count++;
	}

	// Now print the tokenized input as a debug check
	// TODO Remove this code and replace with your shell functionality

	//Checking to see if token is NUll, used for when the enter key is history
	//the loop just starts again.
	if(token[0] == NULL) continue;
	//Checking to see if user inputed exit or quit. This will tell the program to
	//exit.
	if((strcmp(token[0], "exit") == 0)) exit(0);
	if((strcmp(token[0], "quit") == 0)) exit(0);
	//Cheching to see if usering inputed to change directory. There is a couple
	//variations of cd. And thats what those if statements inside are checking.
	if((strcmp(token[0], "cd") == 0))
	{
		if(token[1] == NULL)
		{
			chdir("cd");
			continue;
		}
		if((strcmp(token[1], "..") == 0))
		{
			chdir("..");
			continue;
		}
		else
		{
			chdir(token[1]);
			continue;
		}
	}
	//Checking if user inputed pidhistory. If so, it is just a simple for loop
	//printing out pidhistory array.
	if((strcmp(token[0], "pidhistory") == 0))
	{
		int i;
		for(i = 0; i < pidindex+1; i++)
		{
			printf("%d: %d\n",i,pidhistory[i]);
		}
		continue;
	}
	//Checking if user inputed commandhistory. If so, it is just a simple for loop
	//printing out commandhistory array.
	if((strcmp(token[0], "history") == 0))
	{
		int i;
		for(i = 0; i < commandindex+1; i++)
		{
			printf("%d: %s", i, commandhistory[i]);
		}
		continue;
	}

	//If none of the built in commands are found, then they will run through the
	//fork function.
	pid = fork();

	//Now that we are running fork, I can update pidhistory with pid.
	pidhistory[++pidindex] = pid;

	if( pid == 0 )
	{
		//Checking to see if command is found, if now ret == -1, and it will exit
		//the fork function, and return to the beginning of the while loop.
		//If command is found, command will be executed.
		int ret = execvp(token[0], &token[0]);
		if(ret == -1)
		{
			printf("%s:  command not found\n", token[0]);
			exit(0);
		}
	}
	else
	{
		//Waiting on the child process to finish
		int status = 0;
		wait(&status);
	}

	//Use this to keep track of my index. Since I am only storing 15. Once it goes
	//over 14, I reset it back to 0. So it can start updating the array from top
	//to bottom.
	if(pidindex > 14)
	{
		pidindex = 0;
	}
	if(commandindex > 14)
	{
		commandindex = 0;
	}

	}
	return 0;
	// e2520ca2-76f3-11ec-90d6-0242ac120003
}
