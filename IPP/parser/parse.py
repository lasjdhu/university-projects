from lark import Lark
import sys

OK = 0
ERROR_SCRIPT_PARAM = 10         # Forbidden params combination
ERROR_SCRIPT_INPUT = 11         # Error opening input file for reading
ERROR_SCRIPT_OUTPUT = 12        # Error opening output file for writing
ERROR_LEXICAL = 21              # Lexical error in SOL25 source code
ERROR_SYNTAX = 22               # Syntax error in SOL25 source code
ERROR_SEMANTIC_MAIN = 31        # Missing Main class or its method run
ERROR_SEMANTIC_UNDEFINED = 32   # Undefined var, formal param, class, method
ERROR_SEMANTIC_ARITY = 33       # Bad arity of the block (instance method def)
ERROR_SEMANTIC_COLLISION = 34   # Local var is in collision with formal param
ERROR_INTERNAL = 99             # Internal error (others)


SOL25_GRAMMAR = """
program: class program                      # 1. Program → Class Program
       |                                    # 2. Program → ε
class: "class" CID ":" CID "{" method "}"   # 3. Class → class ⟨Cid⟩ : ⟨Cid⟩ { Method }
method: selector block method               # 4. Method → Selector Block Method
      |                                     # 5. Method → ε
selector: ID                                # 6. Selector → ⟨id⟩
        | ID_COLON selectortail             # 7. Selector → ⟨id:⟩ SelectorTail
selectortail: ID_COLON selectortail         # 8. SelectorTail → ⟨id:⟩ SelectorTail
            |                               # 9. SelectorTail → ε
block: "[" blockpar "|" blockstat "]"       # 10. Block → [ BlockPar | BlockStat ]
blockpar: COLON_ID blockpar                 # 11. BlockPar → ⟨:id⟩ BlockPar
        |                                   # 12. BlockPar → ε
blockstat: ID ":=" expr "." blockstat       # 13. BlockStat → ⟨id⟩ := Expr . BlockStat
         |                                  # 14. BlockStat → ε
expr: exprbase exprtail                     # 15. Expr → ExprBase ExprTail
exprtail: ID                                # 16. ExprTail → ⟨id⟩
        | exprsel                           # 17. ExprTail → ExprSel
exprsel: ID_COLON exprbase exprsel          # 18. ExprSel → ⟨id:⟩ ExprBase ExprSel
       |                                    # 19. ExprSel → ε
exprbase: INT                               # 20.* ExprBase → ⟨int⟩ | ⟨str⟩ | ⟨id⟩ | ⟨Cid⟩
        | STR                               # 20.
        | ID                                # 20.
        | CID                               # 20.
        | block                             # 21. ExprBase → Block
        | "(" expr ")"                      # 22. ExprBase → ( Expr )

CID: /[A-Z][a-zA-Z_0-9]*/
ID: /[a-z_][a-zA-Z_0-9]*/
ID_COLON: ID ":"
COLON_ID: ":" ID

COMMENT: /"[^"]*"/

%import common.INT
%import common.ESCAPED_STRING -> STR
%import common.WS

%ignore WS
%ignore COMMENT
"""


def print_usage():
    usage_text = """
Usage: python parse.py [--help]

Description:
    This script of type filter reads the SOL25 source code from stdin,
    checks the code for lexical, syntactic, and static semantic correctness,
    and prints the XML representation of the abstract syntax tree to stdout.

Options:
    --help  Displays this help message and exit.
"""
    print(usage_text)


def main():
    if len(sys.argv) > 1:
        if sys.argv[1] == '--help':
            print_usage()
            sys.exit(OK)
        else:
            print("Unknown argument:", sys.argv[1])
            sys.exit(ERROR_SCRIPT_PARAM)

    p = Lark(SOL25_GRAMMAR, start="program", parser="lalr")

    try:
        ast = p.parse(sys.stdin.read())
    except Exception as e:
        print("Syntax error:", e)
        sys.exit(ERROR_SYNTAX)

    print(ast)


if __name__ == "__main__":
    main()
