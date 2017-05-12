/* modul myopen.h */

/* Sicherstellung nur einmal engebungen */
#ifndef MYOPEN_H
#define MYOPEN_H

 #include <stdio.h>



/* Konstantendeklarationen */


/* Datenstrukturen */


/* Funktionen deklarationen */

FILE * mypopen(const char * command, const char * type);
int mypclose(FILE * stream);


#endif
