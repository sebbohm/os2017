/**
*
* Beispiel 2
*
* @author Maria Kanikova <ic16b002@technikum-wien.at>
* @author Christian Fuhry <ic16b055@technikum-wien.at>
* @author Sebastian Boehm <ic16b032@technikum-wien.at>
*
* @date 2017/05
* @todo 
* https://cis.technikum-wien.at/documents/bic/2/bes/semesterplan/lu/beispiel2.html
* TAG DER ARBEIT (OS)
*/

#include "mypopen.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

static FILE * fp = NULL;
static pid_t pid = -1;
/**
 *
 * \Mit Hilfe der Funktionen mypopen() und mypclose() können Sie relativ einfach ein Shell-Kommando ausführen und das Ergebnis direkt in ein Programm einlesen und weiterverarbeiten bzw. Daten, aus einem Programm heraus, an dieses Kommando übergeben. 
 *

 *
 */
FILE * mypopen(const char * command, const char * type)
{
	
	int fd[2];
	

	errno = 0;

	if (fp != NULL)
	{
		errno = EAGAIN;
		return NULL;
	}

	if (command == NULL)
	{
		errno = EINVAL;
		return NULL;
	}


	if (pipe(fd) < 0)
	{
		return NULL;
	}
	

	///////// working





	switch (pid = fork())
	{
	case  -1:

	case  0:

	default:
	}

    //mypopen() muß zunächst eine Pipe einrichten (pipe(2)) 
    //und dann einen Kindprozeß generieren (fork(2)). Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren 
    //(dup2(2)) und das im 1. Argument spezifizierte Kommando auszuführen (execl(3) oder execv(3)). 
    //Verwenden Sie - wie die Funktion popen(3) - zum Ausführen des Kommandos die Shell sh(1). 
    //Als letztes muß mypopen() von einem Filedeskriptor einen passenden FILE * mit fdopen(3) erzeugen. 
 
   return ///working
}

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
			return -1;
		}
	}

	pid = -1;
	fp = NULL;

	if (WIFEXITED(status))				//Makro: WIFEXITED(status) Ist TRUE, wenn sich ein Kindprozess normal beendet hat.
	{	
		return WEXITSTATUS(status);		//Makro: WEXITSTATUS(status) Genauer Rückgabewert vom Kindprozess
	}

	errno = ECHILD;		// No child processes (POSIX.1
   
    return -1;
}

