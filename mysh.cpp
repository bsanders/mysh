/* * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Author: Bill Sanders
 * Date: 09/30/2012
 * CS 433: Assignment 2
 * Description:
 * Implementation file for a unix shell
 * Uses fork() and exec() to implement a looping shell to execute arbitrary commands
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <linux/limits.h>
#include <cerrno>
#include "stack.h"

using namespace std;

// forks the process and exec's another program.
int spawn(char* executable, char** arg_list);

// tokenizes a string into a set of fields in a vector
void tokenize(vector <string>& result, string in_str, string delims);

// converts a string-vector into a c-string style array
char ** vectorToStringArray(const vector <string>& args);

// changes the current working directory (uses chdir())
void changeDir(string directory);


int main() 
{
	// cmd is the input string from the user; replaced with each loop.
	string cmd = "";

	// fields is a nice easy to work with vector of strings which will hold the args.
	// I've always thought vector was a terrible name for this class.
	// I mean... It's just a freakin list.
	// A vector is a single entity with multiple, potentially *different* datatypes.
	vector <string> fields;
	// arg_list is the pointer to a c-string array execvp() expects.
	char ** arg_list;

	// Stack which will contain pushd/popd directories.
	Stack dirStack;

	const char * username = getenv("USER");
	if (username != 0)
	{
		// TODO error checking...
	}
	const char * homedir = getenv("HOME");
	if (homedir != 0)
	{
		// TODO error checking...
	}

	// We'll loop until "exit"
	while (cmd != "exit")
	{
		// Note that PATH_MAX actually doesn't mean much of anything.
		// Paths *can* be longer than this on Unix/Linux.
		// But 4096 characters ought to be enough for anybody.
		char current_path[PATH_MAX];
		getcwd(current_path, PATH_MAX);
		// Print a nice prompt for the user.
		printf("%s@%s> ", username, current_path);

		// Then get their input for parsing.
		getline(cin, cmd);
		
		// If command starts with exit, break right away.  Don't bother tokenizing.
		if (cmd.find("exit") == 0)
		{
			break;
		}

		// Otherwise split the cmd string up into arguments along whitespace.
		tokenize(fields, cmd, " \t");
		
		// User entered only whitespace-type characters.  Jerk-move.  Re-loop.
		// How on Earth does C++ STL lack a trim() function for strings???
		if (fields.size() == 0)
		{
			continue;
		}
		// We handle cd, pushd, popd, and dirs ourself.
		else if (fields[0] == "cd")
		{
			string directory;
			// allows the user to just type "cd" to go home.
			if (fields.size() == 1)
			{
				directory = "";
			}
			// otherwise use the first argument after 'cd' as the directory
			else
			{
				directory = fields[1];
			}
			// either way, pass it to the wrapper function
			changeDir(directory);
		}
		// pushd pushes the current working dir to the stack
		// then changes to the directory passed
		else if ((fields[0] == "pushd") && (fields.size() > 1))
		{
			// TODO add try/catch here.
			dirStack.push(current_path);
			changeDir(fields[1]);
		}
		// popd pops a directory off the path, and then switches to it.
		else if (fields[0] == "popd")
		{
			string directory;
			dirStack.pop(directory);
			changeDir(directory);
		}
		// dirs just displays all directories in the stack.  Boring.
		// Why can't STL's stack have a display function?  Or at least iterators.  Lame!
		else if (fields[0] == "dirs")
		{
			dirStack.displayAll();
		}
		// Finally, if we get here, we try to call an executable binary somewhere in the system.
		else
		{
			// Convert the vector args to the crazy c-string array pointer business execvp() wants.
			arg_list = vectorToStringArray(fields);
			// Spawn a child process ignoring the returned child process ID.
			// Cast the arguments to force them to be "const"; removes a compiler warning
			spawn((char *) arg_list[0], (char **)arg_list);
		}
	}

	// kthxbai.
	printf ("Good bye.\n"); 

	return 0; 
}


// Helper functions!

// Spawns a child process to run a new program
// executable is a c-style string of the program name
// arg_list is a pointer to a null-terminated array of c-strings
// returns the PID of the spawned process.
int spawn(char* executable, char** arg_list)
{
	pid_t child_pid;

	child_pid = fork();
	if (child_pid != 0)
	{
		// This is the parent process
		int p_status = 0;
		// Wait for the process to change state
		wait(&p_status);
		return child_pid;
	}
	else
	{
		// attempt to run the executable; note execvp() searches the $PATH
		execvp(executable, arg_list);
		// The function will only return on an error (such as program not found)
		fprintf(stderr, "an error occurred in execvp\n"); 
		abort();
	} 
} 

// Based off of algorithm found at, with modifications:
// http://www.cplusplus.com/faq/sequences/strings/split/#getline
// Accepts a reference to a string-vector (in which the args will be stored),
// an input string (to be tokenized)
// a string containing all delimiters to break tokens across.
void tokenize(vector <string>& result, string in_str, string delims)
{
	result.clear();
	size_t current;
	size_t next = -1;
	do
	{
		next = in_str.find_first_not_of(delims, next + 1);
		if (next == string::npos)
		{
			// break out of the loop as soon as we find the end.
			break;
		}
		else
		{
			next = next - 1;
		}
		current = next + 1;
		next = in_str.find_first_of(delims, current);
		string result_arg = in_str.substr(current, next - current);
		result.push_back(result_arg);
	} while (next != string::npos);
}

// Converts a vector of strings to the infamous null-terminated array of c-strings
// accepts a vector of strings
// returns a pointer to the null-termina-etc-etc-etc.
char ** vectorToStringArray(const vector <string>& args)
{
	// if args is empty, we obviously have nothing to do here.
	// should never be the case in this program....
	int argCount = args.size();
	if ( argCount == 0 )
	{
		return NULL;
	}
	char **buf = new char*[argCount + 1];
	for (int i = 0; i < argCount; ++i)
	{
		buf[i] = new char [args[i].size() + 1];
		strcpy(buf[i], args[i].c_str());
	}
	buf[argCount] = NULL;
	return buf;
}

// Wrapper for the system call chdir
// Accepts a directory path as a string.
// Does not return, but will print error messages to STDOUT
void changeDir(string directory)
{
	// If dir is empty or ~, the user wants their home directory
	if ((directory == "") || (directory == "~"))
	{
		directory = getenv("HOME");
	}

	// system call chdir(), checking for errors.
	int chdir_status = chdir(directory.c_str());

	// If there was an error, print the error message.
	if (chdir_status < 0)
	{
		printf("%s : %s\n", directory.c_str(), strerror(errno));
	}
}

