import sys
from xml.dom.minidom import parseString
import xml.etree.ElementTree as ET
from lark import (
    Lark,
    UnexpectedCharacters, UnexpectedToken,
    Token, Tree
)

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
        | ESCAPED_STRING                    # 20.
        | ID                                # 20.
        | CID                               # 20.
        | block                             # 21. ExprBase → Block
        | "(" expr ")"                      # 22. ExprBase → ( Expr )

CID: /[A-Z][a-zA-Z_0-9]*/
ID: /[a-z_][a-zA-Z_0-9]*/
ID_COLON: ID ":"
COLON_ID: ":" ID

COMMENT: /"[^"]*"/

%import common (INT, ESCAPED_STRING, WS)

%ignore WS
%ignore COMMENT
"""


def print_and_exit(msg, exit_code, output=sys.stdout):
    try:
        print(msg, file=output)
    except IOError:
        exit(ERROR_SCRIPT_OUTPUT)
    exit(exit_code)


def parse_args():
    if len(sys.argv) == 2:
        if sys.argv[1] == '--help':
            print_and_exit(USAGE_TEXT, OK)
        else:
            print_and_exit(f"Unknown argument: {sys.argv[1]}",
                           ERROR_SCRIPT_PARAM, sys.stderr)
    elif len(sys.argv) < 0 or len(sys.argv) > 2:
        print_and_exit("Too many arguments", ERROR_SCRIPT_PARAM, sys.stderr)


def parse_source_code():
    try:
        source_code = sys.stdin.read()
    except IOError:
        print_and_exit("Can't read from stdin", ERROR_SCRIPT_INPUT, sys.stderr)

    try:
        comments = []
        p = Lark(SOL25_GRAMMAR, start="program", parser="lalr",
                 lexer_callbacks={"COMMENT": comments.append})
        ast = p.parse(source_code)
        return ast, comments
    except UnexpectedCharacters as e:
        print_and_exit(f"Lexical error: {e}", ERROR_LEXICAL, sys.stderr)
    except UnexpectedToken as e:
        print_and_exit(f"Syntax error: {e}", ERROR_SYNTAX, sys.stderr)
    except Exception as e:
        print_and_exit(f"Internal error: {e}", ERROR_INTERNAL, sys.stderr)


def create_xml_tree(node, comments=None):
    try:
        if isinstance(node, Token):
            element = ET.Element(node.type)
            element.text = node.value
            return element
        elif isinstance(node, Tree):
            element = ET.Element(node.data)

            match node.data:
                case "program":
                    if node.children == []:
                        return
                    element.set("language", "SOL25")
                    if comments:
                        first_comment = comments[0][1:-1].replace("\n",
                                                                  "&nbsp;")
                        element.set("description", first_comment)
                case "class":
                    cid_tokens = [child for child in node.children if isinstance(
                        child, Token) and child.type == "CID"]

                    if len(cid_tokens) >= 1:
                        element.set("name", cid_tokens[0].value)
                    if len(cid_tokens) >= 2:
                        element.set("parent", cid_tokens[1].value)

                    node.children = [
                        child for child in node.children if child not in cid_tokens]

            for child in node.children:
                child_element = create_xml_tree(child, comments)
                if child_element is not None:
                    element.append(child_element)

            return element
    except Exception as e:
        print_and_exit(f"Internal error: {e}", ERROR_INTERNAL, sys.stderr)


def prettify(xml_tree):
    try:
        rough_string = ET.tostring(xml_tree)
        pretty_xml_string = parseString(rough_string).toprettyxml(indent="  ")
        return ET.ElementTree(ET.fromstring(pretty_xml_string))
    except Exception as e:
        print_and_exit(f"Internal error: {e}", ERROR_INTERNAL, sys.stderr)


if __name__ == "__main__":
    parse_args()

    ast, comments = parse_source_code()
    xml_tree = create_xml_tree(ast, comments)
    pretty_xml_tree = prettify(xml_tree)

    try:
        pretty_xml_tree.write(
            sys.stdout.buffer, encoding="utf-8", xml_declaration=True)
    except IOError:
        print_and_exit("Can't write to stdout",
                       ERROR_SCRIPT_OUTPUT, sys.stderr)

    exit(OK)
