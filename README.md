This variant of [RefPerSys](http://refpersys.org/) uses libgccjit (see https://gcc.gnu.org/ and https://gcc.gnu.org/onlinedocs/jit/ ...) with a simple GNU readline interface.


##intuition

Code quickly something which is able to load and dump a persistent
state in textual format (probably ours) and modify or generate some C
files.

We need a readline interface and the ability to extract and modify
some C files.

As soon as possible provide some libgccjit code generating binary code
(and doing so at load time).