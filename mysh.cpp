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

void split(vector <string> & result, string in_str, string delims)
{
	result.clear();
	size_t current;
	size_t next = -1;
	do
	{
		next = in_str.find_first_not_of(delims, next + 1);
		if (next == string::npos)
		{
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

char **CreateArgVFromVector(const vector<string>& allArgs)
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

int main() 
{
	vector <string> fields;
	
	string cmd = "";
	char ** arg_list;

	const char * username = getenv("USER");
	if (username != 0)
		cout << username << endl;
	const char * homedir = getenv("HOME");
	if (homedir != 0)
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
		

		split(fields, cmd, " ");

		if (fields[0] == "cd")
		{
			// allows the user to just type "cd" to go home.
			if (fields.size() == 1)
			{
				fields.push_back(homedir);
			} // allows the user to type "cd ~" to go home.
			else if (fields[1] == "~")
			{
				fields[1] = string(homedir);
			}

			int chdir_status = chdir(fields[1].c_str());
			if (chdir_status < 0)
			{
				cout << fields[1] << ": " << strerror(errno) << endl;
			}
		}
		else
		{
			// Spawn a child process running the "ls" command. Ignore the returned child process ID.
			// Cast the arguments to force them to be "const"; removes a compiler warning
			arg_list = CreateArgVFromVector(fields);
			spawn((char *) arg_list[0], (char **)arg_list);
		}
	}

	printf ("Good bye.\n"); 

	return 0; 
}
