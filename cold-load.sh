#!/bin/bash -x
# SPDX-License-Identifier: GPL-3.0-or-later
# file cold-load.sh in github.com/RefPerSys/gccjit-refpersys
./gccjit-refpersys --verbose --load $0
exit

## below lines are ignored
useless stuff
see persist.c file routine load_state_RPS and start_comment_RPS

#*START-GCCJIT-REFPERSYS
@MAKOBJ_1WPRD63GJB9S3_KBU3FE
"ab\n01234"
1234
0x675
# line 16
"0123456789_0123456789_0123456789€0123456789É0123456789_0123456789_0123456789_0123456789°0123456789"
@should_do_something 123
# line 19
@echo 'message in line 20
#****************
#**                           for Emacs...
#** Local Variables: ;;
#** compile-command: "make -j2 gccjit-refpersys && ./cold-load.sh" ;;
#** End: ;;
#****************
#### end of file cold-load.sh in github.com/RefPerSys/gccjit-refpersys
