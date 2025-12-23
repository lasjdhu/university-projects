# Password checker

## October 2021

### Description

The goal of the project is to create a program that receives a set of passwords
as an input and verifies for each of them whether the password meets all (fixed)
required rules. Those passwords that pass the check will be printed on output,
others will be discarded from it.

### Detailed specifications

Implement the program in the source file "main.c". Input data (list of
passwords) will be read from standard input (stdin), output (filtered list of
passwords) will be printed to standard output (stdout).

### GCC syntax

`$ gcc -std=c99 -Wall -Wextra -Werror main.c -o main`

### Run syntax

The program is launched in the following form: (./main indicates the location
and name of the program):

`./main LEVEL PARAM [--stats]`

The program starts with two fixed arguments LEVEL and PARAM and with one
optional argument --stats, possibly entered in the third position:

- `LEVEL` an integer in the interval [1, 4] that specifies the desired security
  level (see below)
- `PARAM` a positive integer that specifies an additional rule parameter (see
  below)
- `--stats`, if specified, specifies whether summary statistics of analyzed
  passwords should be printed at the end of the program

### Filters

A total of 4 security levels are defined, expressed using 4 rules. The security
level specifies that passwords must comply with all rules at that level and
below. That is for example, security level 3 specifies that passwords must
comply with rules 1, 2, and 3.

Some rules are parameterizable with an integer specified using the PARAM program
argument. In the following list, this parameter is marked as X. List of rules:

1. Password contains at least 1 uppercase and 1 lowercase letter.
2. The password contains characters from at least X groups (if X is greater than
   4, it means all groups). The groups considered are:
   - lowercase letters (a-z)
   - capital letters (A-Z)
   - numbers (0-9)
   - special characters (at least non-alphanumeric characters from the ASCII
     table in positions 33-126 32-126, i.e. including spaces) must be supported
3. The password does not contain a sequence of identical characters of length at
   least X.
4. The password does not contain two identical substrings of length at least X.

### Statistics

If the `--stats` program argument is specified, the program must print the total
statistics at the end of the output in the format:

```
Statistics:
Different symbols: NCHARS
Min length of password: MIN
Averege length of password: AVG
```

where `NCHARS` is the number of different characters occurring across all
passwords, `MIN` is the length of the shortest password(s), and `AVG` is the
average password length (arithmetic mean) rounded to 1 decimal place. Statistics
also include passwords that have been discarded.

### Implementation details

##### Input data (password list)

The list of passwords is passed to the program on standard input (stdin). Each
password is entered on a separate line and contains only ASCII text data, except
for the newline character. The maximum password length is 100 characters,
otherwise the data is invalid. The program must support an unlimited number of
input passwords.

##### Program output

The program prints to standard output (stdout) the passwords from the input
list, each on a separate line, that meet the required security level specified
as the program's LEVEL argument. Passwords must be entered without change and in
the same order in which they appeared at the input.

After the output list of passwords, the program optionally displays statistics
(see Statistics).

##### Limitations in the project

It is forbidden to use the following functions:

- calling functions from the string.h and ctype.h libraries - the goal of the
  project is to learn how to implement the given functions manually,
- calls from the malloc and free family - work with dynamic memory is not needed
  in this project,
- calls from the fopen, fclose, fscanf, ... family - working with (temporary)
  files is not desirable in this project,
- calling the exit function - the goal of the project is to learn how to create
  program constructions that can handle an unexpected state of the program and
  eventually terminate the program properly by returning from the main function.

##### Unexpected behavior

Respond to program runtime errors in the usual way: respond to unexpected input
data, input data format, or function call errors by interrupting the program
with a brief and concise error message on the appropriate output and a
corresponding return code. Reports will be in Czech or English ASCII encoding.

### Examples of inputs and outputs

```
$ cat pw.txt
1234567890
Password
Heslo123
Mojevelmidlouhehesloscislem0
IZP2021:fit@vut.cz

./main 1 1 <pw.txt
Password
Heslo123
Mojevelmidlouhehesloscislem0
IZP2021:fit@vut.cz
```

```
./main 2 3 <pw.txt
Heslo123
Mojevelmidlouhehesloscislem0
IZP2021:fit@vut.cz
```

```
./main 3 2 <pw.txt
Heslo123
Mojevelmidlouhehesloscislem0
IZP2021:fit@vut.cz
```

```
./main 4 2 <pw.txt
Heslo123
IZP2021:fit@vut.cz
```

```
./main 2 4 --stats <pw.txt
IZP2021:fit@vut.cz
Statistics:
Different symbols: 36
Min length of password: 8
Averege length of password: 14.4
```
