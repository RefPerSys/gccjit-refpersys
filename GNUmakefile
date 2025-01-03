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
##      © Copyright 2024 The Reflective Persistent System Team
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
CC=/usr/bin/gcc
GITID:= $(shell ./do-generate-gitid.sh)
SHORTGITID:= $(shell ./do-generate-gitid.sh -s)
SOURCEDIR:= $(shell /bin/pwd)
CFLAGS= -Wall -Wextra -g -O -std=gnu99 -I/usr/local/include \
        -DSHORTGITID=\"$(SHORTGITID)\" -DSOURCEDIR=\"$(SOURCEDIR)\"

RM= /bin/rm -vf
.PHONY: all clean tests test00

all: gccjit-refpersys

clean:
	$(RM) *.o *.i *~ gccjit-refpersys a.out

gccjit-refpersys: main.o persist.o
	$(CC) $^ -lbsd -lgccjit -L/usr/local/lib  -lunistring  -lbacktrace -ldl -lreadline -lz -o $@


test00: gccjit-refpersys GNUmakefile cold-load.sh
	./cold-load.sh
