# Programmer's doc

The source code is divided into four parts - tokenizer, scope, builtinfunctions and interpreter.

## tokenizer

Class `processStream` provides abstraction of characters.

For this purpose, there's enum type `charGroup` defined:

 * alphanum
 * semicolon
 * newline
 * whitespace
 * op
 * slash
 * backslash
 * quote
 * comma
 * open
 * close
 * eof
 * hash
 * star
 * invalid

`processStream` takes istream in constructor and provides methods move and moveBack for moving in the istream. Method getCurrent returns the `charGroup` to which current character belongs and flush return the actuall char.


`ProcessStream` is used in the class `tokenizer` which takes input and based on the `charGroup` divides input into tokens.
Tokens are again enum class defined as:

 * symbol
 * skip
 * argument
 * terminator
 * nextArgument
 * endOfBlock
 * openArguments
 * closeArguments
 * invalidCharacter



`Tokenizer` contains finite state automaton as depicted in the picture bellow:

```
tokenizer - returns token by token
Boxflow
box is a group of states in DFA
       |---> BlockEnd
       |
WC1 -->+---> Terminator --> back to WC1
       |
       |---> Symbol -> WC2 -+-> Open -- NWC1 -> Argument -> NWC2 --+-> Close --+
                            |         ^                            |           |
                    WC1 <---+         +-<---------- Comma <--------+    WC1 <--+
```

States of the machine are instances of class `box` which implements `getNextBox()` to get the following state of the machine and `parseToken()` which returns `nextToken()`, the corresponding string remains in `processString` and subsequently can be read by `tokenizer` via `flush()`.


`Tokenizer` is inherited from class `TokenizerBase`. Another children of `TokenizerBase` is `TokenizerBuilder` which is a simple wrapper around vector, for situations where we have vector of tokens and we need to pass it to a class that accepts only `tokenizer`.

It provides constructor which takes `std::vector<std::pair<token, std::string>>` and methods `addToken(token, const std::string&)` and `addTokens(std::vector<std::pair<token, std::string>>`.


Tokens from `tokenizer` can be read through methods `nextToken()` which returns next token and `flush` returning the corresponding string to the last token.


## scope

Scope provides class `Object` which is the base class for all types (`primitiveType`, `Function`). It acts as a slot - it contains slot in which this one is containted (controlled via `addUpperSlot`, `getUpperSlot`); and contains nested slots (methods `getSlot()`, `AddIntoSlot(std::string name, std::shared_ptr<Object> object)`). Plus it contains the information if it is callable and method clone which returns a pointer to a new object created with deep copy.

Child of `Object` is `Function` which only defines `operator()` which calls function passed in the constructor.


Another part of scope is class `Arguments`. It is used for easier work with anything in nested parethesis. It takes `tokenizerBase` and reads all tokens until it finds such a sequence that there's one more closing than opening round bracket.

On top of that, it is able to execute the loaded content in the following ways:

* execute it as it is - `execute(obj_ptr& scope)`

* execute it, but use first n-1 arguments as input variables for method and set them to values passed args\_values (`execute(obj_ptr& scope, Arguments& args_values)`).

* `execute_curr_part` which execute only the code until the nextArgument token (which is simply comma separating arguments in between round brackets).


## builtinfunctions

This file provides some basic types and built in functions.

All the following methods takes the same arguments (std::shared\_pointer<Object> scope, Arguments& args) and are to be passed in the `Function` constructor:

* print
* operators - +, ++, (for full see user doc)
* cond - built-in for if
* while\_ - built-in for while
* for\_ - built-in for for
* createMethod - built-in for method - on a call it creates a new instance of class Method and returns it


## Interpreter

Defines class `Interpreter`, which takes `tokenizerBase` and execute the tokens. Internally is divided into `processSymbol` which takes care of all `token::symbol` and `runBlock()` which simply run Interpreter recursively any time ( is found.


Furthermore, there's method main defined which simply creates `iffstream` from a given file and executes interpreter on it.
