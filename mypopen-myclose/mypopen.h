/**
#ifndef MYPOPEN
#define MYPOPEN


* Beispiel 2
 

* @author Maria Kanikova <ic16b002@technikum-wien.at>
* @author Christian Fuhry <ic16b055@technikum-wien.at>
* @author Sebastian Boehm <ic16b032@technikum-wien.at>



/*
 * -------------------------------------------------------------- includes --
 */

#include <stdio.h>

/*
 * --------------------------------------------------------------- defines --
 */

/*
 * -------------------------------------------------------------- typedefs --
 */

/*
 * --------------------------------------------------------------- globals --
 */

/*
 * ------------------------------------------------------------- functions --
 */
extern FILE *mypopen(const char *command, const char *type);
extern int mypclose(FILE *stream);


#endif //MYPOPEN
