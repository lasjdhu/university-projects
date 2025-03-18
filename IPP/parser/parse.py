import sys
from xml.dom.minidom import parseString
import xml.etree.ElementTree as ET
from lark import Lark, UnexpectedCharacters, UnexpectedToken, Token, Tree

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
ERROR_SEMANTIC_OTHER = 35       # Semantic error (other)
ERROR_INTERNAL = 99             # Internal error (other)

RESERVED_WORDS = ["class", "self", "super", "nil", "true", "false"]
VALID_CIDS = ["Object", "Nil", "True", "False", "Integer", "String", "Block"]

CLASS_METHOD_NEW = "new"
CLASS_METHOD_FROM = "from"
CLASS_METHOD_READ = "read"

USAGE_TEXT = """
Usage: python parse.py [-h | --help]

Description:
    This script of type filter reads the SOL25 source code from stdin,
    checks the code for lexical, syntactic, and static semantic correctness,
    and prints the XML representation of the abstract syntax tree to stdout.

Options:
    -h, --help  Displays this help message and exit.
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


class XML:
    def __init__(self):
        self.all_classes = {}
        self.active_class = None
        self.active_method = None
        self.local_vars = {}

    def build_xml(self, parse_tree, comment_list):
        root = ET.Element("program", language="SOL25")

        if comment_list:
            description = comment_list[0].value.strip('"').replace("\n", "&nbsp;")
            root.set("description", description)

        self._find_classes(parse_tree)
        self._verify_main_class()
        self._build_xml_tree(parse_tree, root)

        return root

    def _find_classes(self, program_node):
        for node in program_node.children:
            if node.data == "class":
                class_name = self._get_node_value(node.children[0])
                parent_name = self._get_node_value(node.children[1])

                if class_name in self.all_classes:
                    print_and_exit(
                        "Semantic other error", ERROR_SEMANTIC_OTHER, sys.stderr
                    )

                self.all_classes[class_name] = {
                    "parent": parent_name,
                    "methods": {},
                }

                self._find_methods(node.children[2], class_name)
            elif node.data == "program":
                self._find_classes(node)

    def _find_methods(self, method_node, class_name):
        current = method_node
        while current and current.children:
            selector = self._extract_selector(current.children[0])
            block_node = current.children[1]

            if selector in self.all_classes[class_name]["methods"]:
                print_and_exit("Semantic other error", ERROR_SEMANTIC_OTHER, sys.stderr)

            params = []
            if block_node.children[0].children:
                params = self._extract_params(block_node.children[0])

            self.all_classes[class_name]["methods"][selector] = {
                "arity": len(params),
            }

            if len(current.children) > 2:
                current = current.children[2]
            else:
                break

    def _verify_main_class(self):
        visited = set()
        for class_name in self.all_classes:
            self._check_circular_inheritance(class_name, visited, set())

        if "Main" not in self.all_classes:
            print_and_exit("Semantic main error", ERROR_SEMANTIC_MAIN, sys.stderr)

        if "run" not in self.all_classes["Main"]["methods"]:
            print_and_exit("Semantic main error", ERROR_SEMANTIC_MAIN, sys.stderr)

        if self.all_classes["Main"]["methods"]["run"]["arity"] > 0:
            print_and_exit("Semantic arity error", ERROR_SEMANTIC_ARITY, sys.stderr)

    def _check_circular_inheritance(self, class_name, visited, path):
        if class_name in path:
            print_and_exit("Semantic other error", ERROR_SEMANTIC_OTHER, sys.stderr)

        if class_name in visited:
            return

        if class_name not in self.all_classes:
            return

        visited.add(class_name)
        path.add(class_name)

        parent = self.all_classes[class_name]["parent"]
        self._check_circular_inheritance(parent, visited, path)

        path.remove(class_name)

    def _get_node_value(self, node):
        if isinstance(node, Token):
            return node.value
        elif (
            isinstance(node, Tree)
            and node.children
            and isinstance(node.children[0], Token)
        ):
            return node.children[0].value
        else:
            return ""

    def _build_xml_tree(self, node, parent_elem):
        if not isinstance(node, Tree):
            return

        node_type = node.data
        handler_name = f"_handle_{node_type}"
        handler = getattr(self, handler_name, None)

        if handler:
            handler(node, parent_elem)

    def _handle_program(self, node, parent_elem):
        for child in node.children:
            self._build_xml_tree(child, parent_elem)

    def _handle_class(self, node, parent_elem):
        class_name = self._get_node_value(node.children[0])
        parent_name = self._get_node_value(node.children[1])

        if parent_name not in self.all_classes and parent_name not in VALID_CIDS:
            print_and_exit(
                "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
            )

        class_elem = ET.SubElement(
            parent_elem, "class", attrib={"name": class_name, "parent": parent_name}
        )
        self.active_class = class_name

        method_node = node.children[2]
        self._build_xml_tree(method_node, class_elem)

    def _handle_method(self, node, parent_elem):
        if not node.children:
            return

        selector_node = node.children[0]
        block_node = node.children[1]

        selector = self._extract_selector(selector_node)
        self.active_method = selector
        method_elem = ET.SubElement(
            parent_elem, "method", attrib={"selector": selector}
        )

        self.local_vars = {}

        self._build_xml_tree(block_node, method_elem)

        if len(node.children) > 2:
            self._build_xml_tree(node.children[2], parent_elem)

    def _extract_selector(self, selector_node):
        if selector_node.data != "selector":
            return ""

        if len(selector_node.children) == 1:
            selector = self._get_node_value(selector_node.children[0])
            self._check_reserved_word(selector)
            return selector

        selector = ""
        id_colon = selector_node.children[0]
        id_part = self._get_node_value(id_colon).rstrip(":")
        selector = id_part + ":"

        if len(selector_node.children) > 1:
            selector_tail = selector_node.children[1]
            while selector_tail.children:
                id_colon = selector_tail.children[0]
                id_part = self._get_node_value(id_colon).rstrip(":")
                selector += id_part + ":"
                if len(selector_tail.children) > 1:
                    selector_tail = selector_tail.children[1]
                else:
                    break

        return selector

    def _check_reserved_word(self, word):
        if word in RESERVED_WORDS:
            print_and_exit("Syntax error", ERROR_SYNTAX, sys.stderr)

    def _handle_block(self, node, parent_elem):
        blockpar_node = node.children[0]
        blockstat_node = node.children[1]

        parent_scope = self.local_vars.copy()
        self.local_vars = {}

        params = self._extract_params(blockpar_node)
        block_elem = ET.SubElement(
            parent_elem, "block", attrib={"arity": str(len(params))}
        )

        for param_name in params:
            self.local_vars[param_name] = "parameter"

        self._add_params_to_xml(block_elem, params)
        self._process_statements(block_elem, blockstat_node)
        self.local_vars = parent_scope

    def _add_params_to_xml(self, block_elem, params):
        for i, param_name in enumerate(params, 1):
            ET.SubElement(
                block_elem, "parameter", attrib={"order": str(i), "name": param_name}
            )

    def _process_statements(self, block_elem, blockstat_node):
        statements = self._extract_statements(blockstat_node)
        for i, (var_name, expr_node) in enumerate(statements, 1):
            if var_name in self.local_vars and self.local_vars[var_name] == "parameter":
                print_and_exit(
                    "Semantic collision error", ERROR_SEMANTIC_COLLISION, sys.stderr
                )

            self.local_vars[var_name] = "variable"

            assign_elem = ET.SubElement(block_elem, "assign", attrib={"order": str(i)})
            ET.SubElement(assign_elem, "var", attrib={"name": var_name})
            expr_elem = ET.SubElement(assign_elem, "expr", attrib={})
            self._build_xml_tree(expr_node, expr_elem)

    def _extract_statements(self, blockstat_node):
        statements = []
        current = blockstat_node
        while current.children:
            if current.data == "blockstat" and current.children:
                var_name = self._get_node_value(current.children[0])
                self._check_reserved_word(var_name)
                expr_node = current.children[1]
                statements.append((var_name, expr_node))

                if len(current.children) > 2:
                    current = current.children[2]
                else:
                    break
            else:
                break
        return statements

    def _extract_params(self, blockpar_node):
        params = []
        param_set = set()

        current = blockpar_node
        while current.children:
            if current.data == "blockpar" and current.children:
                colon_id_node = current.children[0]
                param_name = self._get_node_value(colon_id_node).lstrip(":")
                self._check_reserved_word(param_name)

                if param_name in param_set:
                    print_and_exit(
                        "Semantic other error", ERROR_SEMANTIC_OTHER, sys.stderr
                    )

                param_set.add(param_name)
                params.append(param_name)

                if len(current.children) > 1:
                    current = current.children[1]
                else:
                    break
            else:
                break
        return params

    def _handle_expr(self, node, parent_elem):
        exprbase_node = node.children[0]
        self._build_xml_tree(exprbase_node, parent_elem)

        if len(node.children) > 1:
            exprtail_node = node.children[1]
            if not exprtail_node.children:
                return

            tail_child = exprtail_node.children[0]

            if isinstance(tail_child, Token) and tail_child.type == "ID":
                self._check_reserved_word(tail_child.value)
                self._create_simple_send(parent_elem, tail_child.value)
            elif (
                isinstance(tail_child, Tree)
                and tail_child.data == "exprsel"
                and tail_child.children
            ):
                self._create_complex_send(parent_elem, tail_child)

    def _is_subclass_of(self, class_name, parent_name):
        if class_name == parent_name:
            return True

        if class_name not in self.all_classes:
            return False

        current_parent = self.all_classes[class_name]["parent"]
        return self._is_subclass_of(current_parent, parent_name)

    def _create_simple_send(self, parent_elem, selector):
        receiver_elem = parent_elem.find("*")
        if (
            receiver_elem is not None
            and receiver_elem.tag == "literal"
            and receiver_elem.get("class") == "class"
        ):
            class_name = receiver_elem.get("value")
            if selector == CLASS_METHOD_READ:
                if not self._is_subclass_of(class_name, "String"):
                    print_and_exit(
                        "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                    )
            elif selector == CLASS_METHOD_NEW:
                if class_name not in VALID_CIDS:
                    print_and_exit(
                        "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                    )
            else:
                print_and_exit(
                    "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                )

        send_elem = ET.SubElement(parent_elem, "send", attrib={"selector": selector})
        expr_elem = ET.SubElement(send_elem, "expr", attrib={})

        for child in list(parent_elem):
            if child != send_elem:
                expr_elem.append(child)
                parent_elem.remove(child)

    def _create_complex_send(self, parent_elem, exprsel_node):
        receiver_elem = parent_elem.find("*")
        selector, args = self._extract_selector_and_args(exprsel_node)

        if (
            receiver_elem is not None
            and receiver_elem.tag == "literal"
            and receiver_elem.get("class") == "class"
        ):
            class_name = receiver_elem.get("value")
            if selector == CLASS_METHOD_FROM + ":" and class_name != "Integer":
                print_and_exit(
                    "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                )
            if selector not in [CLASS_METHOD_FROM + ":"]:
                print_and_exit(
                    "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                )

        send_elem = ET.SubElement(parent_elem, "send", attrib={"selector": selector})
        expr_elem = ET.SubElement(send_elem, "expr", attrib={})

        for child in list(parent_elem):
            if child != send_elem:
                expr_elem.append(child)
                parent_elem.remove(child)

        self._add_args_to_xml(send_elem, args)
        self._check_self_method_arity(send_elem, selector, len(args))

    def _add_args_to_xml(self, send_elem, args):
        for i, arg_node in enumerate(args, 1):
            arg_elem = ET.SubElement(send_elem, "arg", attrib={"order": str(i)})
            expr_elem = ET.SubElement(arg_elem, "expr", attrib={})
            self._build_xml_tree(arg_node, expr_elem)

    def _check_self_method_arity(self, send_elem, selector, arg_count):
        receiver_elem = send_elem.find("expr/*")
        if (
            receiver_elem is not None
            and receiver_elem.tag == "var"
            and receiver_elem.get("name") == "self"
        ):
            if (
                self.active_class
                and selector in self.all_classes[self.active_class]["methods"]
            ):
                expected_arity = self.all_classes[self.active_class]["methods"][
                    selector
                ]["arity"]
                if expected_arity != arg_count:
                    print_and_exit(
                        "Semantic arity error", ERROR_SEMANTIC_ARITY, sys.stderr
                    )

    def _extract_selector_and_args(self, exprsel_node):
        selector = ""
        args = []

        current = exprsel_node
        while current.children:
            if current.data == "exprsel" and current.children:
                if len(current.children) >= 2:
                    id_colon_node = current.children[0]
                    arg_node = current.children[1]

                    id_part = self._get_node_value(id_colon_node).rstrip(":")
                    self._check_reserved_word(id_part)
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

        return selector, args

    def _handle_exprbase(self, node, parent_elem):
        base_child = node.children[0]

        if isinstance(base_child, Token):
            if base_child.type == "ID":
                var_name = base_child.value
                if var_name not in RESERVED_WORDS and var_name not in self.local_vars:
                    print_and_exit(
                        "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                    )
            elif base_child.type == "CID":
                class_name = base_child.value
                if class_name not in self.all_classes and class_name not in VALID_CIDS:
                    print_and_exit(
                        "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                    )

            self._add_literal_to_xml(parent_elem, base_child)
        else:
            self._build_xml_tree(base_child, parent_elem)

    def _add_literal_to_xml(self, parent_elem, token):
        token_type = token.type
        token_value = token.value

        if token_type == "INT":
            ET.SubElement(
                parent_elem,
                "literal",
                attrib={"class": "Integer", "value": token_value},
            )
        elif token_type == "ESCAPED_STRING" or token_type == "SINGLE_STRING":
            value = token_value[1:-1]
            ET.SubElement(
                parent_elem, "literal", attrib={"class": "String", "value": value}
            )
        elif token_type == "ID":
            self._handle_id_token(parent_elem, token_value)
        elif token_type == "CID":
            ET.SubElement(
                parent_elem, "literal", attrib={"class": "class", "value": token_value}
            )

    def _handle_id_token(self, parent_elem, token_value):
        if token_value == "nil":
            ET.SubElement(
                parent_elem, "literal", attrib={"class": "Nil", "value": "nil"}
            )
        elif token_value == "true":
            ET.SubElement(
                parent_elem, "literal", attrib={"class": "True", "value": "true"}
            )
        elif token_value == "false":
            ET.SubElement(
                parent_elem, "literal", attrib={"class": "False", "value": "false"}
            )
        else:
            ET.SubElement(parent_elem, "var", attrib={"name": token_value})


def print_and_exit(msg, exit_code, output=sys.stdout):
    try:
        print(msg, file=output)
        exit(exit_code)
    except IOError:
        exit(ERROR_SCRIPT_OUTPUT)


def parse_args():
    if len(sys.argv) == 2:
        if sys.argv[1] == "--help" or sys.argv[1] == "-h":
            print_and_exit(USAGE_TEXT, OK, sys.stdout)
        else:
            print_and_exit(
                f"Unknown argument: {sys.argv[1]}", ERROR_SCRIPT_PARAM, sys.stderr
            )
    elif len(sys.argv) > 2:
        print_and_exit("Too many arguments", ERROR_SCRIPT_PARAM, sys.stderr)


def validate_string(token):
    value = token.value
    i = 0
    while i < len(value):
        if value[i] == "\n":
            print_and_exit("Lexical error", ERROR_LEXICAL, sys.stderr)
        elif value[i] == "\\":
            if i + 1 >= len(value):
                print_and_exit("Lexical error", ERROR_LEXICAL, sys.stderr)
            if value[i + 1] not in ["'", "\\", "n"]:
                print_and_exit("Lexical error", ERROR_LEXICAL, sys.stderr)
            i += 2
        else:
            i += 1
    return token


def parse_source_code():
    source_code = ""
    try:
        source_code = sys.stdin.read()
    except IOError:
        print_and_exit("Can't read from stdin", ERROR_SCRIPT_INPUT, sys.stderr)

    try:
        comments = []
        p = Lark(
            SOL25_GRAMMAR,
            start="program",
            parser="lalr",
            lexer_callbacks={
                "COMMENT": comments.append,
                "SINGLE_STRING": validate_string,
            },
        )
        ast = p.parse(source_code)

        xml_tree = XML().build_xml(ast, comments)

        return xml_tree
    except UnexpectedCharacters as e:
        print_and_exit(f"Lexical error: {e}", ERROR_LEXICAL, sys.stderr)
    except UnexpectedToken as e:
        print_and_exit(f"Syntax error: {e}", ERROR_SYNTAX, sys.stderr)
    except Exception as e:
        print_and_exit(f"Internal error: {e}", ERROR_INTERNAL, sys.stderr)


def prettify(xml_tree):
    try:
        rough_string = ET.tostring(xml_tree)
        pretty_string = parseString(rough_string).toprettyxml(
            indent="   ", encoding="UTF-8"
        )
        return ET.fromstring(pretty_string)
    except Exception as e:
        print_and_exit(f"Internal error: {e}", ERROR_INTERNAL, sys.stderr)


if __name__ == "__main__":
    parse_args()

    xml_tree = parse_source_code()
    pretty_xml_tree = prettify(xml_tree)

    try:
        ET.ElementTree(pretty_xml_tree).write(
            sys.stdout.buffer, encoding="UTF-8", xml_declaration=True
        )
        exit(OK)
    except IOError:
        print_and_exit("Can't write to stdout", ERROR_SCRIPT_OUTPUT, sys.stderr)

