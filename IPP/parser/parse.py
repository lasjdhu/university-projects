import sys
import io
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
        |                                   # 2. Program → ε
class: "class" CID ":" CID "{" method "}"   # 3. Class → class ⟨Cid⟩ : ⟨Cid⟩ { Method }
method: selector block method               # 4. Method → Selector Block Method
        |                                   # 5. Method → ε
selector: ID                                # 6. Selector → ⟨id⟩
        | ID_COLON selectortail             # 7. Selector → ⟨id:⟩ SelectorTail
selectortail: ID_COLON selectortail         # 8. SelectorTail → ⟨id:⟩ SelectorTail
        |                                   # 9. SelectorTail → ε
block: "[" blockpar "|" blockstat "]"       # 10. Block → [ BlockPar | BlockStat ]
blockpar: COLON_ID blockpar                 # 11. BlockPar → ⟨:id⟩ BlockPar
        |                                   # 12. BlockPar → ε
blockstat: ID ":=" expr "." blockstat       # 13. BlockStat → ⟨id⟩ := Expr . BlockStat
        |                                   # 14. BlockStat → ε
expr: exprbase exprtail                     # 15. Expr → ExprBase ExprTail
exprtail: ID                                # 16. ExprTail → ⟨id⟩
        | exprsel                           # 17. ExprTail → ExprSel
exprsel: ID_COLON exprbase exprsel          # 18. ExprSel → ⟨id:⟩ ExprBase ExprSel
        |                                   # 19. ExprSel → ε
exprbase: INT                               # 20.* ExprBase → ⟨int⟩ | ⟨str⟩ | ⟨id⟩ | ⟨Cid⟩
        | ESCAPED_STRING                    # 20.
        | SINGLE_STRING                     # 20.
        | ID                                # 20.
        | CID                               # 20.
        | block                             # 21. ExprBase → Block
        | "(" expr ")"                      # 22. ExprBase → ( Expr )

CID: /[A-Z][a-zA-Z_0-9]*/
ID: /[a-z_][a-zA-Z_0-9]*/
ID_COLON: ID ":"
COLON_ID: ":" ID

SINGLE_STRING: /'[^'\\\\]*(\\\\.[^'\\\\]*)*'/

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
        if sys.argv[1] == '--help' or sys.argv[1] == '-h':
            print_and_exit(USAGE_TEXT, OK, sys.stdout)
        else:
            print_and_exit(f"Unknown argument: {sys.argv[1]}",
                           ERROR_SCRIPT_PARAM, sys.stderr)
    elif len(sys.argv) < 0 or len(sys.argv) > 2:
        print_and_exit("Too many arguments", ERROR_SCRIPT_PARAM, sys.stderr)


def parse_source_code():
    source_code = ""
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

    return [], []


def create_xml_tree(ast, comments):
    classes = {}
    root = ET.Element("program", language="SOL25")

    if comments:
        comment = comments[0].value.strip('"')
        comment = comment.replace('\n', "&#10;")
        root.set("description", comment)

    process_ast_node(ast, root, classes)

    if "Main" not in classes:
        print_and_exit("Missing Main class", ERROR_SEMANTIC_MAIN, sys.stderr)

    main_class = classes["Main"]
    if "run" not in main_class["methods"]:
        print_and_exit("Missing run method in Main class", ERROR_SEMANTIC_MAIN, sys.stderr)

    return root

def process_ast_node(node, parent_elem, classes, context=None):
    if not isinstance(node, Tree):
        return

    node_type = node.data

    if node_type == "program":
        for child in node.children:
            process_ast_node(child, parent_elem, classes)
    
    elif node_type == "class":
        class_name = get_token_value(node.children[0])
        parent_name = get_token_value(node.children[1])

        class_elem = ET.SubElement(parent_elem, "class", name=class_name, parent=parent_name)
        classes[class_name] = {"parent": parent_name, "methods": {}}

        method_node = node.children[2]
        process_ast_node(method_node, class_elem, classes, {"class": class_name})

    elif node_type == "method":
        if not node.children:
            return

        selector_node = node.children[0]
        block_node = node.children[1]

        selector = ""
        if selector_node.data == "selector":
            if len(selector_node.children) == 1:
                selector = get_token_value(selector_node.children[0])
            else:
                id_colon = selector_node.children[0]
                selector = get_token_value(id_colon).rstrip(':') + ":"

                if len(selector_node.children) > 1:
                    selector_tail = selector_node.children[1]
                    while selector_tail.children:
                        id_colon = selector_tail.children[0]
                        selector += get_token_value(id_colon).rstrip(':') + ":"
                        if len(selector_tail.children) > 1:
                            selector_tail = selector_tail.children[1]
                        else:
                            break

        method_elem = ET.SubElement(parent_elem, "method", selector=selector)

        class_name = context.get("class") if context else None
        if class_name and class_name in classes:
            classes[class_name]["methods"][selector] = {}

        process_ast_node(block_node, method_elem, classes)

        if len(node.children) > 2:
            process_ast_node(node.children[2], parent_elem, classes, context)

    elif node_type == "block":
        blockpar_node = node.children[0]
        blockstat_node = node.children[1]

        params = []
        current = blockpar_node
        while current.children:
            if current.data == "blockpar" and current.children:
                colon_id_node = current.children[0]
                param_name = get_token_value(colon_id_node).lstrip(':')
                params.append(param_name)

                if len(current.children) > 1:
                    current = current.children[1]
                else:
                    break
            else:
                break

        block_elem = ET.SubElement(parent_elem, "block", arity=str(len(params)))

        for i, param_name in enumerate(params, 1):
            ET.SubElement(block_elem, "parameter", order=str(i), name=param_name)

        statements = []
        current = blockstat_node
        while current.children:
            if current.data == "blockstat" and current.children:
                var_name = get_token_value(current.children[0])
                expr_node = current.children[1]
                statements.append((var_name, expr_node))

                if len(current.children) > 2:
                    current = current.children[2]
                else:
                    break
            else:
                break

        for i, (var_name, expr_node) in enumerate(statements, 1):
            assign_elem = ET.SubElement(block_elem, "assign", order=str(i))
            ET.SubElement(assign_elem, "var", name=var_name)
            expr_elem = ET.SubElement(assign_elem, "expr")
            process_ast_node(expr_node, expr_elem, classes)

    elif node_type == "expr":
        exprbase_node = node.children[0]
        process_ast_node(exprbase_node, parent_elem, classes)

        if len(node.children) > 1:
            exprtail_node = node.children[1]
            if not exprtail_node.children:
                return

            tail_child = exprtail_node.children[0]

            if isinstance(tail_child, Token) and tail_child.type == "ID":
                send_elem = ET.SubElement(parent_elem, "send", selector=tail_child.value)
                expr_elem = ET.SubElement(send_elem, "expr")

                for child in list(parent_elem):
                    if child != send_elem:
                        expr_elem.append(child)
                        parent_elem.remove(child)

            elif isinstance(tail_child, Tree) and tail_child.data == "exprsel" and tail_child.children:
                selector = ""
                args = []

                current = tail_child
                while current.children:
                    if current.data == "exprsel" and current.children:
                        if len(current.children) >= 2:
                            id_colon_node = current.children[0]
                            arg_node = current.children[1]

                            id_part = get_token_value(id_colon_node).rstrip(':')
                            selector += id_part + ":"

                            args.append(arg_node)

                            if len(current.children) > 2:
                                current = current.children[2]
                            else:
                                break
                        else:
                            break
                    else:
                        break

                send_elem = ET.SubElement(parent_elem, "send", selector=selector)
                expr_elem = ET.SubElement(send_elem, "expr")

                for child in list(parent_elem):
                    if child != send_elem:
                        expr_elem.append(child)
                        parent_elem.remove(child)

                for i, arg_node in enumerate(args, 1):
                    arg_elem = ET.SubElement(send_elem, "arg", order=str(i))
                    expr_elem = ET.SubElement(arg_elem, "expr")
                    process_ast_node(arg_node, expr_elem, classes)

    elif node_type == "exprbase":
        base_child = node.children[0]

        if isinstance(base_child, Token):
            token_type = base_child.type
            token_value = base_child.value

            if token_type == "INT":
                ET.SubElement(parent_elem, "literal", attrib={"class": "Integer", "value": token_value})
            elif token_type == "ESCAPED_STRING" or token_type == "SINGLE_STRING":
                value = token_value[1:-1]
                ET.SubElement(parent_elem, "literal", attrib={"class": "String", "value": value})
            elif token_type == "ID":
                if token_value == "nil":
                    ET.SubElement(parent_elem, "literal", attrib={"class": "Nil", "value": "nil"})
                elif token_value == "true":
                    ET.SubElement(parent_elem, "literal", attrib={"class": "True", "value": "true"})
                elif token_value == "false":
                    ET.SubElement(parent_elem, "literal", attrib={"class": "False", "value": "false"})
                else:
                    ET.SubElement(parent_elem, "var", name=token_value)
            elif token_type == "CID":
                ET.SubElement(parent_elem, "literal", attrib={"class": "class", "value": token_value})
        else:
            process_ast_node(base_child, parent_elem, classes)

def get_token_value(node):
    if isinstance(node, Token):
        return node.value
    elif isinstance(node, Tree) and node.children and isinstance(node.children[0], Token):
        return node.children[0].value
    else:
        return ""

def prettify(xml_tree):
    try:
        rough_string = ET.tostring(xml_tree, encoding="UTF-8")
        pretty_string = parseString(rough_string).toprettyxml(indent="   ", encoding="UTF-8")
        return ET.fromstring(pretty_string)
    except Exception as e:
        print_and_exit(f"Internal error: {e}", ERROR_INTERNAL, sys.stderr)


if __name__ == "__main__":
    parse_args()

    ast, comments = parse_source_code()
    xml_tree = create_xml_tree(ast, comments)
    pretty_xml_tree = prettify(xml_tree)

    try:
        buffer = io.BytesIO()
        ET.ElementTree(pretty_xml_tree).write(buffer, encoding="UTF-8", xml_declaration=True)
        sys.stdout.buffer.write(buffer.getvalue())
    except IOError:
        print_and_exit("Can't write to stdout", ERROR_SCRIPT_OUTPUT, sys.stderr)

    exit(OK)
