##
## @file Makefile
## Betriebssysteme mypopen Makefile
## Beispiel 2
##
## @author 
## @author 
## @date 2005/02/22
##
## @version $Revision: 1689 $
##
##	@versuch 1 Anpassung an mypopen von Steinmetz, Posch, Stanescu
##
##
## @todo
##
## URL: $HeadURL: https://svn.petrovitsch.priv.at/ICSS-BES/trunk/2015/CIS/lu/hello/Makefile $
##
## Last Modified: $Author: tom $
##

##
## ------------------------------------------------------------- variables --
##

CC=gcc52
##CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -fno-common -g -O3 -std=gnu11
CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes -fno-common -g -O3 -std=gnu11 -lpopentest

CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

##OBJECTS=mypopen.c mypopen.h mytest.c
OBJECTS=mypopen.c mypopen.h 

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

##%.o: %.c 
##		$(CC)  -c $< $(CFLAGS)

##
## --------------------------------------------------------------- targets --
##


all: popentest

.PHONY: clean
clean:
	$(RM) *.o *~ mypopen

popentest: $(OBJECTS)
		$(CC) $(CFLAGS) -o $@ $^
		
.PHONY: distclean
distclean: clean
	$(RM) -r doc

doc: html pdf

.PHONY: html
html:
	$(DOXYGEN) doxygen.dcf

pdf: html
	$(CD) doc/pdf && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile && \
	$(MV) refman.save refman.pdf

#popentest: popentest
#		$(CC) $(CFLAGS) -o mypopen.o -lpopentest

		

##   $(OBJECTS)
##	$(CC) $(CFLAGS) $@ $^


## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##