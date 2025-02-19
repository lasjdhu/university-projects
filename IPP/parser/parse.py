from lark import Lark, UnexpectedCharacters, UnexpectedToken, Token
import xml.etree.ElementTree as ET
from xml.dom.minidom import parseString
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

USAGE_TEXT = """
Usage: python parse.py [--help]

Description:
    This script of type filter reads the SOL25 source code from stdin,
    checks the code for lexical, syntactic, and static semantic correctness,
    and prints the XML representation of the abstract syntax tree to stdout.

Options:
    --help  Displays this help message and exit.
"""


SOL25_GRAMMAR = """
program: class program                      # 1. Program → Class Program
       |                                    # 2. Program → ε
# 3. Class → class ⟨Cid⟩ : ⟨Cid⟩ { Method }
class: "class" CID ":" CID "{" method "}"
method: selector block method               # 4. Method → Selector Block Method
      |                                     # 5. Method → ε
selector: ID                                # 6. Selector → ⟨id⟩
        | ID_COLON selectortail             # 7. Selector → ⟨id:⟩ SelectorTail
# 8. SelectorTail → ⟨id:⟩ SelectorTail
selectortail: ID_COLON selectortail
            |                               # 9. SelectorTail → ε
# 10. Block → [ BlockPar | BlockStat ]
block: "[" blockpar "|" blockstat "]"
blockpar: COLON_ID blockpar                 # 11. BlockPar → ⟨:id⟩ BlockPar
        |                                   # 12. BlockPar → ε
# 13. BlockStat → ⟨id⟩ := Expr . BlockStat
blockstat: ID ":=" expr "." blockstat
         |                                  # 14. BlockStat → ε
expr: exprbase exprtail                     # 15. Expr → ExprBase ExprTail
exprtail: ID                                # 16. ExprTail → ⟨id⟩
        | exprsel                           # 17. ExprTail → ExprSel
# 18. ExprSel → ⟨id:⟩ ExprBase ExprSel
exprsel: ID_COLON exprbase exprsel
       |                                    # 19. ExprSel → ε
# 20.* ExprBase → ⟨int⟩ | ⟨str⟩ | ⟨id⟩ | ⟨Cid⟩
exprbase: INT
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


def print_message_and_exit(msg, exit_code):
    print(msg)
    sys.exit(exit_code)


def parse_args():
    if len(sys.argv) == 2:
        if sys.argv[1] == '--help':
            print_message_and_exit(USAGE_TEXT, OK)
        else:
            print_message_and_exit(
                f"Unknown argument: {sys.argv[1]}",
                ERROR_SCRIPT_PARAM
            )
    elif len(sys.argv) < 0 or len(sys.argv) > 2:
        print_message_and_exit("Too many arguments", ERROR_SCRIPT_PARAM)


def parse_source_code():
    try:
        source_code = sys.stdin.read()
    except IOError:
        print_message_and_exit("Can't read from stdin", ERROR_SCRIPT_INPUT)

    try:
        p = Lark(SOL25_GRAMMAR, start="program", parser="lalr")
        return p.parse(source_code)
    except UnexpectedCharacters as e:
        print_message_and_exit(f"Lexical error: {e}", ERROR_LEXICAL)
    except UnexpectedToken as e:
        print_message_and_exit(f"Syntax error: {e}", ERROR_SYNTAX)
    except Exception as e:
        print_message_and_exit(f"Internal error: {e}", ERROR_INTERNAL)


def porduce_output(xml_tree):
    try:
        rough_string = ET.tostring(xml_tree, encoding="unicode")
        p = parseString(rough_string)
        return p.toprettyxml(indent="  ")
    except Exception as e:
        print_message_and_exit(f"Internal error: {e}", ERROR_INTERNAL)


def create_xml_tree(node):
    try:
        if isinstance(node, Token):
            element = ET.Element(node.type)
            element.text = node.value
            return element

        element = ET.Element(node.data)
        for child in node.children:
            element.append(create_xml_tree(child))
        return element
    except Exception as e:
        print_message_and_exit(f"Internal error: {e}", ERROR_INTERNAL)


if __name__ == "__main__":
    parse_args()
    ast = parse_source_code()
    xml_tree = create_xml_tree(ast)
    output = porduce_output(xml_tree)
    print_message_and_exit(output, OK)
