/*
 * @file mypopen.c
 * Betriebssysteme mypopen
 * Beispiel 2
 *
 * @author Karl Heinz Steinmetz, Ieriel Stanescu, Christian Posch
 * @date 2017/04/16
 *
 * @version 1.0.0
 *
 * @todo
 * Programm soll die Funktionen
 * popen
 * und 
 * pclose
 * imitieren
 */

/*
 * -------------------------------------------------------------- includes --
 */

 /* include des eigenen headerfiles */
 #include "mypopen.h"

 #include <unistd.h>
 #include <stdio.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 #include <errno.h>
 #include <string.h>
 #include <stdlib.h>

 /*
 * --------------------------------------------------------------- defines --
 */
 
 
/*
 * -------------------------------------------------------------- typedefs --
 */

/*
 * --------------------------------------------------------------- globals --
 */
	static int child_pid;
	static FILE *given_fp;
/*
 * ------------------------------------------------------------- functions --
 */

/**
 * \brief myopen imitiert popen

 * \param command ist ein NULL terminierter Befehlsstring, der die Shellbefehle beinhaltet.
 * \param type ist bestimmt ob schreibend oder lesend geöffnet wird. Muss ein NULL terminierter String sein, der nur einen Char beinhalten darf. "w" für schreibenden, "r" für lesenden Zugriff.
 *
 * \return FILE
 * \retval Bei Erfolg ein Pointer zu einem FILE Struct ansonst NULL.
 */

 
 FILE *mypopen(const char *command, const char *type)
 {
	 /*verhindern des mehrfachen Aufrufens von mypopen */
	 if (child_pid != 0)
	 {
		 errno = EAGAIN;
		 return NULL;
	 }
	  
	 /* Variablendeklaration für diese Funktion */
	 int fd[2];
	 int direction;
	 pid_t pid;
	 int tmp;

	 /* Je nach Anforderung werden Steuervariablen gesetzt */
	 if (strcmp(type,"r") == 0) direction = 0;
	 else if (strcmp(type, "w")==0) direction = 1;
	 else
	 {
		 /*error da kein gültiger type übergeben wurde*/
		 errno = EINVAL;
		 return NULL;
	 }

	 /* Pipe öffnen, Filedeskriptor zuweisen */
		 /*errorno wird durch pipe gesetzt.*/
	 if (pipe(fd) < 0) return NULL;
	 

	 /* Hier kommt jetzt der FORK! Danach ist entscheidend ob wir Eltern oder Kindprozess sind. */
	 switch(pid = fork())
	 {
	 case -1:		/*ERROR*/
		 (void) close(fd[0]);
	 	 (void) close(fd[1]);
	 	 return NULL;
		 break;
	 case 0:		/*Kindprozess*/
		 // Schließen der nicht benötigten Seite der Pipe
		 if (close(fd[direction]) == -1){
			 (void) close(fd[0]);
			 (void) close(fd[1]);
			 /* Wir sind im Kindprozess, dieser muss mit einem Fehlerwert (!=0) beendet werden. */
			 return EXIT_FAILURE;
		 }
		 // Umleiten der Standardausgabe
		
		if (fd[0] != STDIN_FILENO && fd[1] != STDOUT_FILENO)
		{
			if (direction == 1) tmp = dup2(fd[0], STDIN_FILENO);
			else tmp = dup2(fd[1], STDOUT_FILENO);

			if (tmp == -1)
			{
				/*error*/
				 (void) close(fd[0]);
				 (void) close(fd[1]);
				 /* Wir sind im Kindprozess, dieser muss mit einem Fehlerwert (!=0) beendet werden. */
				 return EXIT_FAILURE;
			}
			(void) close(fd[!direction]);
		}
		
		 /* Verwendung von shell um Übergabe der Parameter zu vereinfachen */
		 (void) execl("/bin/sh", "sh", "-c", command, (char *) NULL);
		 /* Code sollte nie hier her gelangen, bei Fehlern beim Aufruf von execl jedoch schon, dann muss aufgeräumt werden */
		 /* Im Fehlerfall return execl = -1, errno wurde durch execl richtig gesetzt. */
		 given_fp = NULL;
		 child_pid = 0;
		 /* Wir sind im Kindprozess, dieser muss mit einem Fehlerwert (!=0) beendet werden. */
		 exit (-1);
		 break;
	
	 default:		/*Elternprozess*/
		 child_pid = pid;
		 // Filedeskriptor in Filepointer umwandeln
		 given_fp = fdopen(fd[direction], type);
		 if (given_fp == NULL)
		 {
			 /* error */
			 child_pid = 0;
			 (void) close(fd[0]);
			 (void) close(fd[1]);
			 return NULL;
		 }
		 (void) close(fd[!direction]);
		 return given_fp;
	 } 
	 /* Ende switch fork, ab hier wird im Eltern und Kindprozess ausgeführt, falls im Kindprozess das execute nicht funktioniert. */
	 
	 
	/* Letzter return, sollte nie erreicht werden! */
	return NULL;
}


/**
 * \brief Schließt den angegebenen FILE Struct

 * \param stream Pointer auf den zu schließenden FILE Struct
 
 *
 * \return INT
 * \retval Null bei Erfolg ansonsten -1
 */

int mypclose(FILE * stream)
{
	int status;
	pid_t wait_pid;
	/* Wir haben noch kein Child erstellt */
	if (child_pid == 0)
	{
		errno = ECHILD;
		return -1;		
	}
	/* Der Filestream wurde nicht von uns erstellt */
	if (stream != given_fp)
	{
		errno = EINVAL;
		return -1;
	}
	/* Das Schließen des Filestream hat nicht funktioniert */
	if (fclose(stream) != 0)
	{
		return -1;
	}
	/* Hier warten wir auf das Beenden des Kindprozesses */
	while ((wait_pid= waitpid(child_pid, &status, 0)) != child_pid)
	{
		/* Falls Fehler auftreten (Kind existiert nicht, ... ) */
		if (wait_pid == -1)
		{
			/* Falls der Kindprozess nur unterbrochen wurde */
			if (errno == EINTR)
			{
				// /*waitpid() unterbrochen ==> weitermachen */
				continue;
			}
			// /* error */
			child_pid = 0;
			errno = ECHILD;
			return -1;
		}
	}

	/* Kindprozess wurde normal terminiert*/
	if(WIFEXITED(status))
	{
		child_pid = 0;
		given_fp = NULL;
		return WEXITSTATUS(status);
	}
	else
	{
		child_pid = 0;
		errno = ECHILD;
		return -1;
	}
}

/*
 * =================================================================== eof ==
 */
