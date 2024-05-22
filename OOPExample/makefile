# compiling OOPExample with gcc 2.7.2.1
# by David Le Corfec <lecorfec@univ-mlv.fr> (5-Jun-97)
CC = gcc

# -noixemul: needs libnix for its auto-open librairies feature
# -O2      : needs optimisation to use MUI inlines (else needs a libmuistubs.a)
CFLAGS = -noixemul -O2

# some personal (optional) flags ...
RZFLAGS = -fstrength-reduce -fbaserel -msmall-code -Wimplicit \
          -Wstrict-prototypes -Wall -s

OOPExample : OOPExample.c OOPExample.h
    $(CC) $(CFLAGS) $(RZFLAGS) OOPExample.c -o OOPExample_GNU

