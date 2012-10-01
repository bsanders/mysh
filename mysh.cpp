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

/* Spawn a child process running a new program. PROGRAM is the name 
   of the program to run; the path will be searched for this program. 
   ARG_LIST is a NULL-terminated list of character strings to be 
   passed as the program's argument list. Returns  the process ID of 
   the spawned process.  */ 

using namespace std;

int spawn(char* program, char** arg_list) 
{
	pid_t child_pid;

	/* Duplicate this process. */ 
	child_pid = fork (); 
	if (child_pid != 0)
	{
		/* This is the parent process. */
		int p_status = 0;
		wait(&p_status);
		return child_pid;
	}
	else {
		/* Now execute PROGRAM, searching for it in the path.  */ 
		printf("\n");
		int err_status = execvp(program, arg_list);
		/* The execvp  function returns only if an error occurs.  */ 
		fprintf(stderr,  "an error occurred in execvp\n"); 
		abort(); 
	} 
} 

void tokenize(vector <string> & result, string in_str, string delims)
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
//		cout << result_arg << endl;
		result.push_back(result_arg);
	} while (next != string::npos);
}

// void print( vector <string> & v )
// {
// 	for (size_t n = 0; n < v.size(); n++)
// 		cout << "\"" << v[ n ] << "\"\n";
// 	cout << endl;
// }

char **createArgVFromVector(const vector<string>& allArgs)
{
	int nArgs = allArgs.size();
	if ( nArgs == 0 )
		return NULL;
	char **buf = new char*[nArgs + 1];
	for (int i = 0; i < nArgs; ++i)
	{
		buf[i] = new char [allArgs[i].size() + 1];
		strcpy(buf[i], allArgs[i].c_str());
	}
	buf[nArgs] = NULL;
	return buf;
}

void changeDir(string directory)
{
	// If dir is empty or ~, the user means their home directory
	if ((directory == "") || (directory == "~"))
		directory = getenv("HOME");

	// system call chdir(), checking for errors.
	int chdir_status = chdir(directory.c_str());
	if (chdir_status < 0)
	{
		printf("%s : %s\n", directory.c_str(), strerror(errno));
	}
}

int main() 
{
	vector <string> fields;
	Stack dirStack;
	
	string cmd = "";
	char ** arg_list;

	const char * username = getenv("USER");
	if (username != 0)
		// TODO error checking...
		cout << username << endl;
	const char * homedir = getenv("HOME");
	if (homedir != 0)
		// TODO error checking...
		cout << homedir << endl;

	while (cmd != "exit")
	{
		// Note that PATH_MAX actually doesn't mean much of anything.
		// Paths *can* be longer than this on Unix/Linux.
		// But 4096 characters ought to be enough for anybody.
		char current_path[PATH_MAX];
		char * path = getcwd(current_path, PATH_MAX);
		cout << current_path << "> ";

		getline(cin, cmd);
		
		// If command starts with exit, break right away.
		if (cmd.find("exit") == 0)
			break;
		

		tokenize(fields, cmd, " ");

		if (fields[0] == "cd")
		{
			string directory;
			// allows the user to just type "cd" to go home.
			if (fields.size() == 1)
			{
				directory = "";
			}
			// otherwise use the first argument to cd.
			else
			{
				directory = fields[1];
			}
			changeDir(directory);
		}
		// pushd pushes the current working dir to the stack
		// then changes to the directory passed
		else if ((fields[0] == "pushd") && (fields.size() > 1))
		{
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
		// dirs just displays all directories in the stack.
		else if (fields[0] == "dirs")
		{
			dirStack.displayAll();
		}
		else
		{
			// Spawn a child process running the "ls" command. Ignore the returned child process ID.
			// Cast the arguments to force them to be "const"; removes a compiler warning
			arg_list = createArgVFromVector(fields);
			spawn((char *) arg_list[0], (char **)arg_list);
		}
	}

	printf ("Good bye.\n"); 

	return 0; 
}
