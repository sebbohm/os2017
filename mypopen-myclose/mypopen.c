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
    //mypopen() muß zunächst eine Pipe einrichten (pipe(2)) 
    //und dann einen Kindprozeß generieren (fork(2)). Im Kindprozeß ist das richtige Ende der Pipe ("r" oder "w") mit stdin bzw. stdout zu assoziieren 
    //(dup2(2)) und das im 1. Argument spezifizierte Kommando auszuführen (execl(3) oder execv(3)). 
    //Verwenden Sie - wie die Funktion popen(3) - zum Ausführen des Kommandos die Shell sh(1). 
    //Als letztes muß mypopen() von einem Filedeskriptor einen passenden FILE * mit fdopen(3) erzeugen. 
 
   return
}

int mypclose(FILE *stream)
{
 
 //Bei Aufruf von mypclose() soll der aufrufende Prozeß auf die Terminierung des Kindprozesses warten (waitpid(2)). 
 //Zur Vereinfachung soll immer nur höchstens eine Pipe mit mypopen() geöffnet werden können. 
 //Stellen Sie dies in ihrer Implementierung sicher. 
   
    return
}
