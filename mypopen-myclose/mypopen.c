/**
*
* @file mypopen.c
* Betriebssysteme mypopen/myclose c File.
* Beispiel 2 
*
* @author Maria Kanikova <ic16b002@technikum-wien.at>
* @author Christian Fuhry <ic16b055@technikum-wien.at>
* @author Sebastian Boehm <ic16b032@technikum-wien.at>
*
* @date 2017/05
*
*/

/*
* -------------------------------------------------------------- includes --
*/

#include "mypopen.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

/*
* --------------------------------------------------------------- defines --
*/

/*
* -------------------------------------------------------------- typedefs --
*/

/*
* --------------------------------------------------------------- globals --
*/

static FILE * fp = NULL;
static pid_t pid = -1;

/*
* ------------------------------------------------------------- functions --
*/

/**
*
* \brief mypopen part of exercise 2
*
* This function creates a pipe and a child process.
* Depending on read or write choice the end of the pipe is associated with stdin or stdout.
*
* \param command to be executed and forked
* \param type I/O mode read or write
*
* \return NULL or fp
* \retval NULL if erroneous or no pointer to return
* \retval fp filepointer of successfull fork
*
*/
FILE * mypopen(const char * command, const char * type)
{
	
	int fd[2];
	int parent, child;
	errno = 0;

	if (type[0] != 'r' && type[0] != 'w' && command == NULL)
	{
		errno = EINVAL;		//Invalid argument (POSIX.1)
		return NULL;
	}
	
	if (type[1] != '\0')
	{
		errno = EINVAL;
		return NULL;
	}
	
	if (fp != NULL)
	{
		errno = EAGAIN;		//Resource temporarily unavailable
		return NULL;
	}

	if (pipe(fd) < 0)
	{
		return NULL;
	}
	
	switch (type[0])
	{
	case 'r':	parent = 0; 
				child = 1;
				break;
	case 'w':	parent = 1;
				child = 0;
				break;
	default:	errno = EINVAL;
				return NULL;
	}

	switch (pid = fork())
	{
	case  -1:	close(fd[parent]);	//close parent pipe
				close(fd[child]);	//close child pipe
				return NULL;		

	case  0:	close(fd[parent]);	//childprocess
				if (fd[child] != child)
				{
					if (dup2(fd[child], child) == -1)
					{
						close(fd[child]);
						_exit(1);
					}
					close(fd[child]);
				}
				execl("/bin/sh", "sh", "-c", command, (char*) NULL);
				_exit(127); 

	default:	close(fd[child]);	//parentprocess
				if ((fp = fdopen(fd[parent], type)) == NULL) //NULL returned if error occured
				{
					close (fd[parent]);
					return NULL;
				}
				return fp;			
	}
	
   return NULL;
}

/**
*
* \brief mypclose part of exercise 2
*
* This function waits for the termination of a child process.
* Returns -1 on different errors and waits for the child process to be closed.
*
* \param stream to be closed child process
*
* \return -1 or WEXITSTATUS(status)
* \retval -1 if erroneous or no pointer to return
* \retval WEXITSTATUS(status) returnvalue of closed child process
*
*/
int mypclose(FILE *stream)
{
	pid_t child_pid;
	int status;
	errno = 0;

	if (fp == NULL)
	{
		errno = ECHILD;		// No child processes (POSIX.1)
		return -1;
	}
	
		
	if (stream == NULL || fp != stream)
	{
		errno = EINVAL;		//Invalid argument (POSIX.1)
		return -1;
	}


	if (fclose(stream) == EOF)
	{
		fp = NULL;
		pid = -1;
		return -1;
	}

	if (pid <= 0)
	{
		return -1;
	}

	while ((child_pid = waitpid(pid, &status, 0)) != pid)		// waitpid() wartet bis Kindprozess beendet (Wert: -1)
	{
		if (child_pid == -1)
		{
			if (errno == EINTR)		//Interrupted function call
			{				
				continue;
			}
			pid = -1;
			fp = NULL;
			return -1;
		}
	}
	
	pid = -1;
	fp = NULL;

	if (WIFEXITED(status) != 0)				//Makro: WIFEXITED(status) Ist TRUE, wenn sich ein Kindprozess normal beendet hat.
	{	
		return WEXITSTATUS(status);		//Makro: WEXITSTATUS(status) Genauer Rückgabewert vom Kindprozess
	}

	errno = ECHILD;		// No child processes (POSIX.1
   
    return -1;
}

/*
 * ------------------------------------------------------------------- eof --
 */