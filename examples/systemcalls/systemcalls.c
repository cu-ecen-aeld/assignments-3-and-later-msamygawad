#include "systemcalls.h"

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int res = system(cmd);
    bool retrunVal;
    if(res != 0 || cmd == NULL){
        perror("Error : ");
        retrunVal = false;
    }
    else
        retrunVal = true;

    return retrunVal;
    
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
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    bool returnVal = true;

    pid_t pid;
    pid = fork();

    //faild to fork
    if (pid == -1){
        returnVal = false;
        perror("Error : ");
    }

    //sucess to fork and running child process
    else if (pid == 0){
        int exe_ret ;
        exe_ret = execv(command[0], command);
        if (exe_ret == -1)
            exit(EXIT_FAILURE); // error in fucntion execution
    }

    //sucess to fork and running parent  process
    else{
        int wait_ret;
        int child_status;
        wait_ret= waitpid (pid, &child_status, 0);

        //if waitpid() call failed (-1)  OR  return status from child not equal 0 (successful exit)
        if (wait_ret == -1 || WEXITSTATUS(child_status) != 0){
            returnVal = false;
            perror("Error : ");
        }
    }

    va_end(args);

    return returnVal;
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
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];
    

/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT , 0600);
    int exe_ret ;
    int wait_ret;
    int child_status;
    bool returnVal = true;

    pid_t pid;
    pid = fork();

    //faild to fork
    if (pid == -1){
        returnVal = false;
        perror("Error : ");
    }
        
    //sucess to fork and running child process
    else if (pid == 0){
        int dup_ret = dup2(fd,STDOUT_FILENO);
        //if there is a problem in redirection of STD_OUTPUT to file, then terminate the parent process
        if (dup_ret < 0)
            exit(EXIT_FAILURE);
        else{
            close(fd);
            exe_ret = execv(command[0], command);
            if (exe_ret == -1)
                exit(EXIT_FAILURE); // error in fucntion execution
        }
    }

    //sucess to fork and running parent  process
    else{
        wait_ret= waitpid (pid, &child_status, 0);
        //if waitpid() call failed (-1)  OR  return status from child not equal 0 (successful exit)
        if  (wait_ret == -1 || 
            (WIFEXITED(child_status) && WEXITSTATUS(child_status) != 0)){
            perror("Error : ");
            returnVal = false;
        }
    }
    va_end(args);

    return returnVal;
}
