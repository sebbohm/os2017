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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

static FILE * fp = NULL;
/**
 *
 * \Mit Hilfe der Funktionen mypopen() und mypclose() können Sie relativ einfach ein Shell-Kommando ausführen und das Ergebnis direkt in ein Programm einlesen und weiterverarbeiten bzw. Daten, aus einem Programm heraus, an dieses Kommando übergeben. 
 *

 *
 */
FILE * mypopen(const char * command, const char * type)
{
    
 
   return
}

int mypclose(FILE *stream)
{
 
 
    return
}
