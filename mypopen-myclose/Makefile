##==========================================================================
##
## @file Makefile
## Betriebsysteme mypopen Makefile
## Beispiel 2
##
## @author Maria Kanikova  <ic16b002@technikum-wien.at>
## @author Christian Fuhry <ic16b055@technikum-wien.at>
## @author Sebastian Boehm <ic16b032@technikum-wien.at>
##
## @date 2017/05/05
##
## ------------------------------------------------------------- variables --
##

CC=gcc52
CFLAGS=-DDEBUG -Wall -Werror -Wextra -Wstrict-prototypes -pedantic -fno-common -O3 -g -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

OBJECTS=mypopen.o

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

##
## --------------------------------------------------------------- targets --
##

all: mypopen

mypopen: $(OBJECTS)
	$(CC) -o popentest $(OBJECTS) -lpopentest
	$(CC) -o test-pipe $(OBJECTS) -ltest-pipe

local: $(OBJECTS)
	$(CC) $(CFLAGS) -m32 -o popentest $(OBJECTS) libpopentest.a
	$(CC) $(CFLAGS) -m32 -o test-pipe $(OBJECTS) libtest-pipe.a

clean:
	$(RM) *.o *~ mypopen
	$(RM) popentest
	$(RM) test-pipe

distclean: clean
	$(RM) -r doc

doc: html pdf

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

##
## =================================================================== eof ==
##
