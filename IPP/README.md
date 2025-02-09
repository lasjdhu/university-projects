# IPPcode23

## April 2023

## First part: `parse.php`

### Description
A filter script (parse.php in PHP 8.1) reads the source code in custom language "IPPcode23" from the standard input, checks the lexical and syntactic correctness of the code and writes XML representation of the program to standard.

### Usage details
This script will work with the following parameters:
+ `--help`

Parser-specific error return codes:
+ 21 - wrong or missing header in the source code written in IPPcode23;
+ 22 - unknown or incorrect operation code in the source code written in IPPcode23;
+ 23 - other lexical or syntactic error of the source code written in IPPcode23.

Alternatively you can run tests on this script with `php test.php` command inside root tree

CURRENT_STATE: `Tests finished with no failed tests and 1043 passed tests`

## Second part: `interpret.py`

### Description
Interpreter for custom programming language "IPPcode23"

### Usage details
This script will work with the following parameters:
+ `--help` \
At least one of next two is requred, if one is missing, it will be replaced with STDIN content:
+ `[--source]` - for source XML file generated with parse.php
+ `[--input]` - for inputs for specific operations like READ

Interpret-specific error return codes:
+ 52 - error in the semantic checks of the input code in IPPcode23 (e.g. use of undefined of a tag, redefinition of a variable);
+ 53 - runtime interpretation error – wrong types of operands;
+ 54 - runtime interpretation error – access to non-existent variable (frame exists);
+ 55 - runtime interpretation error – the frame does not exist (eg reading from an empty frame stack);
+ 56 - run-time interpretation error – missing value (in a variable, on the data stack or
in the call stack);
+ 57 - run-time interpretation error – wrong operand value (e.g. division by zero, wrong return
value of the EXIT instruction);
+ 58 - runtime interpretation error – incorrect work with the string.
