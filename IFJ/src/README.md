# Description
- `array.c` Dynamic array
- `ASTnodes.c` Abstract syntax tree nodes creation
- `cgen.c` Code generator
- `expr_parser.c` Expression parser (using precedence)
- `jm_string.c` String
- `memory.c` Memory allocation management
- `parser.c` Parser (using LL(1))
- `scanner.c` Scanner
- `stack.c` Stack
- `symtable.c` Symbol table (using hashtable)

# Usage
Built the project:
```console
$ make
...
```

Compile IFJ24 code to three-address code
```console 
$ ./IFJ24comp < input.zig > tac.out
```

After use `ic24int` interpret:
```console
./ic24int tac.out < prog_input.in > prog_out.out
```