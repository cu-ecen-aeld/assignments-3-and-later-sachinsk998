#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "systemcalls.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// ... (rest of your code)


/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
    int code = system(cmd);
    return (code == 0);
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];

    for (int i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }

    command[count] = NULL;

    // Avoid compile warning before implementation is complete
    command[count] = command[count];

    fflush(stdout);
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("Fork failed");
        va_end(args);
        return false;
    }
    else if (pid == 0) // Child process
    {
        if (execv(command[0], command) == -1)
        {
            perror("Execv failed");
            _exit(EXIT_FAILURE);
        }
    }

    int status;
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("Waitpid failed");
        va_end(args);
        return false;
    }

    va_end(args);
    
    return WIFEXITED(status) && (WEXITSTATUS(status) == 0);
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];

    for (int i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }

    command[count] = NULL;

    // Avoid compile warning before implementation is complete
    command[count] = command[count];

    fflush(stdout);
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("Fork failed");
        va_end(args);
        return false;
    }
    else if (pid == 0) // Child process
    {
        int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
        {
            perror("Open failed");
            _exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("Dup2 failed");
            _exit(EXIT_FAILURE);
        }

        close(fd);

        if (execv(command[0], command) == -1)
        {
            perror("Execv failed");
            _exit(EXIT_FAILURE);
        }
    }

    int status;
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("Waitpid failed");
        va_end(args);
        return false;
    }

    va_end(args);
    
    return WIFEXITED(status) && (WEXITSTATUS(status) == 0);
}

