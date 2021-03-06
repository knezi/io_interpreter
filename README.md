Interpreter of a subset of IO language
===========

# compilation

Simply run `make`, which produces `interpreter` executable.

If provided, the first argument is used as input, that is:

`./interpreter prg.io` will interpret file `prg.io`.  

Otherwise `test/testfile.io` is read and interpreter.

# language

This interpretes a subset of [IO language link](http://iolanguage.org/). It supports:

Numbers and Booleans - simply by typing 0,1... and True/False.

Moreover slots:

```
:=                           | creates a new slot assigns right-hand side
----------------------------------------------------------------------------------
=                            | assigns into a slot right-hand side
----------------------------------------------------------------------------------
method(args, code)           | create a new method which can be assigned
                             | into a slot
----------------------------------------------------------------------------------
while(cond, code)            | executes code until cond is False
----------------------------------------------------------------------------------
for(counter, start, stop,    | Internally translated into:
    step, code)              | counter := start;
                             | while(counter != stop, code;
                             |                        counter = counter + step)
----------------------------------------------------------------------------------
if(cond, ifTrue[, ifFalse])  | evaluates cond and run ifTrue or
                             | ifFalse respectively 
----------------------------------------------------------------------------------
print                        | Primitivetypes have defined callable slot print
                             | which prints their values to stdin
----------------------------------------------------------------------------------
<,>,!=,==,<=,>=              | operators returing Bool defined over primitivetypes
----------------------------------------------------------------------------------
++                           | unary operator defined on Number
----------------------------------------------------------------------------------
+,-,*,/                      | binary operators defined over Numbers
```

# Preview of valid code:

```io
cond := 4
if(cond == 2 + 2, 1 print, 2 print)
if(cond != 2, 2 print)
m := method(ar, ar2, ar print; ar2 print)
m(3,2)
a := (1 + 6 * b == 1*(1 + 5 + 7))
a print
(2 ++ + 4 ++ * 2 ++ print) print
aq2 := method(89 print
1 print)
aq2
(1 == 2) print
(1 != 2) print
(1 > 2) print
(1 >= 2) print
(1 <= 2) print
(1 < 1) print
counter := 1
while(counter != 5, counter print; counter = counter + 1 * 2)
for(a, 6, 22, 4 + 2 * 2, a print)
```
