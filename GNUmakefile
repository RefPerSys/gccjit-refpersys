#!/usr/bin/gmake
## SPDX-License-Identifier: GPL-3.0-or-later
## Description:
##      This file is part of the Reflective Persistent System. refpersys.org
##      thre gccjit-refpersys variant
##      It is its GNUmakefile, for the GNU make automation builder.
##
## Author(s):
##      Basile Starynkevitch <basile@starynkevitch.net>
##      Abhishek Chakravarti <abhishek@taranjali.org>
##      Nimesh Neema <nimeshneema@gmail.com>
##      Abdullah Siddiqui <siddiquiabdullah92@gmail.com>
##
##      © Copyright 2024 - 2026 The Reflective Persistent System Team
##      team@refpersys.org
##
## License:
##    This program is free software: you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation, either version 3 of the License, or
##    (at your option) any later version.
##
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program.  If not, see <http://www.gnu.org/licenses/>

export
#CC=/usr/bin/gcc-14
CC=gcc
TINYCC?=tcc
GITID:= $(shell ./do-generate-gitid.sh)
SHORTGITID:= $(shell ./do-generate-gitid.sh -s)
SOURCEDIR:= $(shell /bin/pwd)
OPTIMFLAGS?= -Og
CFLAGS= -Wall -Wextra -Wmissing-prototypes -g $(OPTIMFLAGS) -std=gnu17 -I/usr/local/include \
        -DSHORTGITID=\"$(SHORTGITID)\" -DSOURCEDIR=\"$(SOURCEDIR)\"
CFILES:=$(wildcard *.c)
OBJFILES:=$(patsubst %.c, obj/%.o, $(CFILES))
NONGLOBOBFILES=$(patsubst %.c, obj/%.o, $(filter-out global.c, $(CFILES)))
RM= /bin/rm -vf
INDENT= indent
.PHONY: all optim clean tests indent lto
.SUFFIXES: .c

all: gccjit-refpersys | GNUmakefile

lto: $(CFILES) | GNUmakefile
	$(MAKE) OPTIMFLAGS='-O2 -flto' $(OBJFILES)
	$(MAKE) gccjit-refpersys-lto

optim: $(CFILES) | GNUmakefile
	$(MAKE) OPTIMFLAGS='-O1' $(OBJFILES)
	$(MAKE) OPTIMFLAGS='-O1' gccjit-refpersys

clean:
	$(RM) *.o obj/*.o *.i *~ *% gccjit-refpersys* *.orig a.out

gccjit-refpersys-lto: $(CFILES) | GNUmakefile
	$(CC) -flto $(CFLAGS) $(CFILES)  -lm  -lgccjit -L/usr/local/lib \
	   -lunistring  -lbacktrace -ldl -lreadline -ljemalloc -lz -o $@

gccjit-refpersys-tcc: $(CFILES) | GNUmakefile
	$(TINYCC) -DNO_STATIC_ASSERTS $(CFLAGS) $(CFILES)  -lm  -lgccjit -L/usr/local/lib \
	   -lunistring  -lbacktrace -ldl -lreadline -ljemalloc -lz -o $@

gccjit-refpersys: $(OBJFILES) | GNUmakefile
	$(CC) $(CFLAGS) global.c $(NONGLOBOBFILES) -lm  -lgccjit -L/usr/local/lib  -lunistring  -lbacktrace -ldl -lreadline -ljemalloc -lz -o $@

indent: GNUmakefile $(wildcard *.[ch]) .indent.pro | GNUmakefile
	for f in *.c *.h ; do $(INDENT) -v $$f ; done


test00: gccjit-refpersys GNUmakefile cold-load.sh | GNUmakefile
	./cold-load.sh

tests: test00

obj/%.o: %.c hdgjirps.h  |GNUmakefile 
	$(CC) $(CFLAGS) -DBASENAME=\"$(basename $<)\" -c $< -o $@


#****************
#**                           for Emacs...
#** Local Variables: ;;
#** compile-command: "make -j2 gccjit-refpersys" ;;
#** End: ;;
#****************

### end of file gccjit-refpersys/GNUmakefile
