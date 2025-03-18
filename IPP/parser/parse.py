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

%import common (INT, WS)

%ignore WS
%ignore COMMENT
"""


class SOL25BaseClass:
    """Base class containing common attributes and methods for SOL25 classes"""

    def __init__(self, class_registry=None):
        self.class_registry = class_registry if class_registry is not None else {}
        self.current_class = None
        self.scope_variables = {}

    def update_current_class(self, class_name):
        """Update current class name"""
        self.current_class = class_name

    def update_scope_variables(self, variables):
        """Update scope variables"""
        self.scope_variables = variables

    # Helper functions

    def is_self_reference(self, elem):
        """Check if element is a reference to self"""
        if elem is None:
            return False

        if elem.tag != "var":
            return False

        if elem.get("name") != "self":
            return False

        return True

    def is_class_literal(self, elem):
        """Check if element is a class literal"""
        if elem is None:
            return False

        if elem.tag != "literal":
            return False

        if elem.get("class") != "class":
            return False

        return True


class Validator(SOL25BaseClass):
    """Validator class"""

    def __init__(self, class_registry):
        super().__init__(class_registry)

    def validate_unique_class(self, class_name):
        """Verify class is not already defined"""
        if class_name in self.class_registry:
            print_and_exit("Semantic other error", ERROR_SEMANTIC_OTHER, sys.stderr)

    def validate_unique_method(self, class_name, selector):
        """Verify class method is not already defined"""
        if selector in self.class_registry[class_name]["methods"]:
            print_and_exit("Semantic other error", ERROR_SEMANTIC_OTHER, sys.stderr)

    def validate_main_class_exists(self):
        """Verify Main class is defined"""
        if "Main" not in self.class_registry:
            print_and_exit("Semantic main error", ERROR_SEMANTIC_MAIN, sys.stderr)

    def validate_main_run_method(self):
        """Verify Main class has run method with arity 0"""
        if "run" not in self.class_registry["Main"]["methods"]:
            print_and_exit("Semantic main error", ERROR_SEMANTIC_MAIN, sys.stderr)

        if self.class_registry["Main"]["methods"]["run"]["arity"] > 0:
            print_and_exit("Semantic arity error", ERROR_SEMANTIC_ARITY, sys.stderr)

    def validate_parent_class(self, parent_name):
        """Verify parent class exists"""
        if parent_name not in self.class_registry and parent_name not in VALID_CIDS:
            print_and_exit(
                "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
            )

    def validate_unique_parameter(self, param_name, param_set):
        """Verify parameter name is not duplicate in current scope"""
        if param_name in param_set:
            print_and_exit("Semantic other error", ERROR_SEMANTIC_OTHER, sys.stderr)

    def validate_var_not_parameter(self, var_name):
        """Verify variable name doesn't have a collision with a parameter"""
        if var_name in self.scope_variables:
            if self.scope_variables[var_name] == "parameter":
                print_and_exit(
                    "Semantic collision error", ERROR_SEMANTIC_COLLISION, sys.stderr
                )

    def validate_class_method(self, class_name, selector):
        """Verify class method exists on class"""
        if selector == CLASS_METHOD_READ:
            if not self._check_inheritance(class_name, "String"):
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

    def validate_keyword_class_method(self, class_name, selector):
        """Verify keyword class method exists on class"""
        if selector == CLASS_METHOD_FROM + ":":
            if class_name != "Integer":
                print_and_exit(
                    "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                )
        else:
            print_and_exit(
                "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
            )

    def validate_method_arity(self, send_elem, selector, arg_count):
        """Verify method call has correct number of arguments"""
        receiver_elem = send_elem.find("expr/*")

        # Only validate self sends
        if not self.is_self_reference(receiver_elem):
            return

        # Check arity when calling method on self
        if self.current_class:
            if selector in self.class_registry[self.current_class]["methods"]:
                expected_arity = self.class_registry[self.current_class]["methods"][
                    selector
                ]["arity"]
                if expected_arity != arg_count:
                    print_and_exit(
                        "Semantic arity error", ERROR_SEMANTIC_ARITY, sys.stderr
                    )

    def validate_token_reference(self, token):
        """Verify token references valid variable or class"""
        if token.type == "ID":
            var_name = token.value
            if var_name not in RESERVED_WORDS and var_name not in self.scope_variables:
                print_and_exit(
                    "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                )
        elif token.type == "CID":
            class_name = token.value
            if class_name not in self.class_registry and class_name not in VALID_CIDS:
                print_and_exit(
                    "Semantic undefined error", ERROR_SEMANTIC_UNDEFINED, sys.stderr
                )

    def validate_not_reserved(self, word):
        """Verify word is not a reserved keyword"""
        if word in RESERVED_WORDS:
            print_and_exit("Syntax error", ERROR_SYNTAX, sys.stderr)

    def validate_inheritance_cycles(self):
        """Detect cycles in class inheritance"""
        visited = set()
        for class_name in self.class_registry:
            self._detect_inheritance_cycle(class_name, visited, set())

    # Helper functions

    def _detect_inheritance_cycle(self, class_name, visited, path):
        """Detect circular inheritance (recursive)"""
        if class_name in path:
            print_and_exit("Semantic other error", ERROR_SEMANTIC_OTHER, sys.stderr)

        if class_name in visited or class_name not in self.class_registry:
            return

        visited.add(class_name)
        path.add(class_name)  # Add current class to path to detect cycles

        parent = self.class_registry[class_name]["parent"]
        self._detect_inheritance_cycle(parent, visited, path)

        path.remove(class_name)  # Remove from path after checking

    def _check_inheritance(self, class_name, parent_name):
        """Check if class inherits from specified parent"""
        if class_name == parent_name:
            return True

        if class_name not in self.class_registry:
            return False

        current_parent = self.class_registry[class_name]["parent"]
        return self._check_inheritance(current_parent, parent_name)


class ASTConverter(SOL25BaseClass):
    "Class that takes AST and makes an XML representation of it"

    def __init__(self):
        super().__init__()
        self.current_method = None
        self.validator = Validator(self.class_registry)

    def generate_xml(self, parse_tree, comment_list):
        """Create XML representation of the AST"""
        root = ET.Element("program", language="SOL25")

        if comment_list:
            description = comment_list[0].value.strip('"').replace("\n", "&nbsp;")
            root.set("description", description)

        self.collect_classes(parse_tree)
        self.validator.validate_inheritance_cycles()
        self.validator.validate_main_class_exists()
        self.validator.validate_main_run_method()
        self.process_tree_to_xml(parse_tree, root)

        return root

    def collect_classes(self, program_node):
        """Collect all class definitions"""
        for node in program_node.children:
            if node.data == "class":
                class_name = self.extract_token_value(node.children[0])
                parent_name = self.extract_token_value(node.children[1])

                self.validator.validate_unique_class(class_name)

                self.class_registry[class_name] = {
                    "parent": parent_name,
                    "methods": {},
                }

                self.collect_methods(node.children[2], class_name)
            elif node.data == "program":
                self.collect_classes(node)

    def collect_methods(self, method_node, class_name):
        """Collect all method definitions for a class"""
        methods = self.extract_methods(method_node)

        for method_data in methods:
            selector = method_data[0]
            block_node = method_data[1]

            self.validator.validate_unique_method(class_name, selector)

            params = []
            if block_node.children[0].children:
                params = self.parse_parameters(block_node.children[0])

            self.class_registry[class_name]["methods"][selector] = {
                "arity": len(params),
            }

    def extract_methods(self, method_node):
        """Extract method definitions"""
        methods = []
        current = method_node

        while current and current.children:
            selector_node = current.children[0]
            block_node = current.children[1]

            selector = self.parse_selector(selector_node)
            methods.append((selector, block_node))

            current = current.children[2] if len(current.children) > 2 else None

        return methods

    def extract_token_value(self, node):
        """Get value from token or tree node"""
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

    def process_tree_to_xml(self, node, parent_elem):
        """Process tree node to XML"""
        if not isinstance(node, Tree):
            return

        node_type = node.data
        handler_name = f"process_{node_type}_node"
        handler = getattr(self, handler_name, None)

        if handler:
            handler(node, parent_elem)

    def process_program_node(self, node, parent_elem):
        """Process program node to XML"""
        for child in node.children:
            self.process_tree_to_xml(child, parent_elem)

    def process_class_node(self, node, parent_elem):
        """Process class node to XML"""
        class_name = self.extract_token_value(node.children[0])
        parent_name = self.extract_token_value(node.children[1])

        self.validator.validate_parent_class(parent_name)

        class_elem = ET.SubElement(
            parent_elem, "class", attrib={"name": class_name, "parent": parent_name}
        )
        self.current_class = class_name
        self.validator.update_current_class(class_name)

        method_node = node.children[2]
        self.process_tree_to_xml(method_node, class_elem)

    def process_method_node(self, node, parent_elem):
        """Process method node to XML"""
        if not node.children:
            return

        selector_node = node.children[0]
        block_node = node.children[1]

        selector = self.parse_selector(selector_node)
        self.current_method = selector
        method_elem = ET.SubElement(
            parent_elem, "method", attrib={"selector": selector}
        )

        # Create a new scope for each method
        self.scope_variables = {}
        self.validator.update_scope_variables(self.scope_variables)
        self.process_tree_to_xml(block_node, method_elem)

        # Process next method if exists
        if len(node.children) > 2:
            self.process_tree_to_xml(node.children[2], parent_elem)

    def parse_selector(self, selector_node):
        """Parse method selector from selector node"""
        if selector_node.data != "selector":
            return ""

        # Simple selector (just an identifier)
        if len(selector_node.children) == 1:
            selector = self.extract_token_value(selector_node.children[0])
            self.validator.validate_not_reserved(selector)
            return selector

        # Keyword selector (with colons)
        return self.parse_keyword_selector(selector_node)

    def parse_keyword_selector(self, selector_node):
        """Parse keyword selector with multiple parts"""
        selector_parts = []

        id_colon = selector_node.children[0]
        id_part = self.extract_token_value(id_colon).rstrip(":")
        selector_parts.append(id_part + ":")

        if len(selector_node.children) > 1:
            self.collect_selector_tail_parts(selector_node.children[1], selector_parts)

        return "".join(selector_parts)

    def collect_selector_tail_parts(self, selector_tail, parts):
        """Collect remaining parts of a selector tail"""
        current = selector_tail
        while current and current.children:
            id_colon = current.children[0]
            id_part = self.extract_token_value(id_colon).rstrip(":")
            parts.append(id_part + ":")
            if len(current.children) > 1:
                current = current.children[1]
            else:
                break

    def process_block_node(self, node, parent_elem):
        """Process block node into XML"""
        blockpar_node = node.children[0]
        blockstat_node = node.children[1]

        # Save parent scope and create a new local scope for the block
        parent_scope = self.scope_variables.copy()
        self.scope_variables = {}
        self.validator.update_scope_variables(self.scope_variables)

        params = self.parse_parameters(blockpar_node)
        block_elem = ET.SubElement(
            parent_elem, "block", attrib={"arity": str(len(params))}
        )
        for param_name in params:
            self.scope_variables[param_name] = "parameter"
        self.validator.update_scope_variables(self.scope_variables)

        self.add_parameters_to_xml(block_elem, params)
        self.process_block_statements(block_elem, blockstat_node)

        # Restore parent scope when exiting block
        self.scope_variables = parent_scope
        self.validator.update_scope_variables(self.scope_variables)

    def add_parameters_to_xml(self, block_elem, params):
        """Add parameter elements to XML"""
        for i, param_name in enumerate(params, 1):
            order = str(i)
            self.create_xml_element(
                block_elem, "parameter", {"order": order, "name": param_name}
            )

    def create_xml_element(self, parent, tag, attributes=None, text=None):
        """Create XML element with attributes and text"""
        if attributes is None:
            attributes = {}

        elem = ET.SubElement(parent, tag, attrib=attributes)
        if text:
            elem.text = text
        return elem

    def process_block_statements(self, block_elem, blockstat_node):
        """Process block statements to XML"""
        statements = self.parse_statements(blockstat_node)

        for i, statement in enumerate(statements, 1):
            var_name = statement[0]
            expr_node = statement[1]

            self.validator.validate_var_not_parameter(var_name)
            self.scope_variables[var_name] = "variable"
            self.validator.update_scope_variables(self.scope_variables)

            self.create_assignment_element(block_elem, i, var_name, expr_node)

    def create_assignment_element(self, block_elem, order, var_name, expr_node):
        """Create assignment XML element"""
        assign_elem = self.create_xml_element(
            block_elem, "assign", {"order": str(order)}
        )
        self.create_xml_element(assign_elem, "var", {"name": var_name})
        expr_elem = self.create_xml_element(assign_elem, "expr")
        self.process_tree_to_xml(expr_node, expr_elem)

    def parse_statements(self, blockstat_node):
        """Extract statements from block statement node"""
        statements = []
        current = blockstat_node

        while current and current.children:
            if current.data == "blockstat":
                var_name = self.extract_token_value(current.children[0])
                self.validator.validate_not_reserved(var_name)
                expr_node = current.children[1]
                statements.append((var_name, expr_node))

                if len(current.children) > 2:
                    current = current.children[2]
                else:
                    break
            else:
                break

        return statements

    def parse_parameters(self, blockpar_node):
        """Extract parameters from block parameter node"""
        params = []
        param_set = set()  # Track parameters to detect duplicates

        current = blockpar_node
        while current and current.children:
            if current.data == "blockpar":
                colon_id_node = current.children[0]
                param_name = self.extract_token_value(colon_id_node).lstrip(":")
                self.validator.validate_not_reserved(param_name)
                self.validator.validate_unique_parameter(param_name, param_set)
                param_set.add(param_name)
                params.append(param_name)

                if len(current.children) > 1:
                    current = current.children[1]
                else:
                    break
            else:
                break

        return params

    def process_expr_node(self, node, parent_elem):
        """Process expression node to XML"""
        exprbase_node = node.children[0]
        self.process_tree_to_xml(exprbase_node, parent_elem)

        # Process message send if exists
        if len(node.children) > 1:
            exprtail_node = node.children[1]
            self.process_expression_tail(exprtail_node, parent_elem)

    def process_expression_tail(self, exprtail_node, parent_elem):
        """Process expression message send"""
        if not exprtail_node.children:
            return

        tail_child = exprtail_node.children[0]

        # no arguments
        if isinstance(tail_child, Token) and tail_child.type == "ID":
            self.validator.validate_not_reserved(tail_child.value)
            self.create_unary_message(parent_elem, tail_child.value)

        # with arguments
        elif (
            isinstance(tail_child, Tree)
            and tail_child.data == "exprsel"
            and tail_child.children
        ):
            self.create_keyword_message(parent_elem, tail_child)

    def create_unary_message(self, parent_elem, selector):
        """Create unary message send XML element"""
        receiver_elem = parent_elem.find("*")

        if self.validator.is_class_literal(receiver_elem):
            self.validator.validate_class_method(receiver_elem.get("value"), selector)

        send_elem = self.create_xml_element(parent_elem, "send", {"selector": selector})
        expr_elem = self.create_xml_element(send_elem, "expr")

        self.move_children_to_element(parent_elem, send_elem, expr_elem)

    def create_keyword_message(self, parent_elem, exprsel_node):
        """Create keyword message send XML element"""
        receiver_elem = parent_elem.find("*")
        selector, args = self.parse_selector_and_args(exprsel_node)

        if self.validator.is_class_literal(receiver_elem):
            class_name = receiver_elem.get("value")
            # Check "from:"
            self.validator.validate_keyword_class_method(class_name, selector)

        send_elem = self.create_xml_element(parent_elem, "send", {"selector": selector})
        expr_elem = self.create_xml_element(send_elem, "expr")

        self.move_children_to_element(parent_elem, send_elem, expr_elem)
        self.add_args_to_message(send_elem, args)
        self.validator.validate_method_arity(send_elem, selector, len(args))

    def move_children_to_element(self, source_elem, exclude_elem, target_elem):
        """Move children from source to target except exclude_elem"""
        for child in list(source_elem):
            if child != exclude_elem:
                target_elem.append(child)
                source_elem.remove(child)

    def add_args_to_message(self, send_elem, args):
        """Add argument elements to message send"""
        for i, arg_node in enumerate(args, 1):
            arg_elem = self.create_xml_element(send_elem, "arg", {"order": str(i)})
            expr_elem = self.create_xml_element(arg_elem, "expr")
            self.process_tree_to_xml(arg_node, expr_elem)

    def parse_selector_and_args(self, exprsel_node):
        """Extract selector and arguments from keyword message"""
        selector_parts = []
        args = []

        current = exprsel_node
        while current and current.children:
            if current.data == "exprsel":
                if len(current.children) >= 2:
                    id_colon_node = current.children[0]
                    arg_node = current.children[1]

                    id_part = self.extract_token_value(id_colon_node).rstrip(":")
                    self.validator.validate_not_reserved(id_part)
                    selector_parts.append(id_part + ":")
                    args.append(arg_node)

                    if len(current.children) > 2:
                        current = current.children[2]
                    else:
                        break
                else:
                    break
            else:
                break

        return "".join(selector_parts), args

    def process_exprbase_node(self, node, parent_elem):
        """Process exprbase node to XML"""
        base_child = node.children[0]

        if isinstance(base_child, Token):
            self.validator.validate_token_reference(base_child)
            self.create_literal_element(parent_elem, base_child)
        else:
            self.process_tree_to_xml(base_child, parent_elem)

    def create_literal_element(self, parent_elem, token):
        """Create XML element for a literal value"""
        token_type = token.type
        token_value = token.value

        if token_type == "INT":
            self.create_xml_element(
                parent_elem,
                "literal",
                {"class": "Integer", "value": token_value},
            )
        elif token_type == "SINGLE_STRING":
            value = token_value.strip("'")
            self.create_xml_element(
                parent_elem, "literal", {"class": "String", "value": value}
            )
        elif token_type == "ID":
            self.process_id_token(parent_elem, token_value)
        elif token_type == "CID":
            self.create_xml_element(
                parent_elem, "literal", {"class": "class", "value": token_value}
            )

    def process_id_token(self, parent_elem, token_value):
        """Process identifier token to XML"""
        if token_value == "nil":
            self.create_xml_element(
                parent_elem, "literal", {"class": "Nil", "value": "nil"}
            )
        elif token_value == "true":
            self.create_xml_element(
                parent_elem, "literal", {"class": "True", "value": "true"}
            )
        elif token_value == "false":
            self.create_xml_element(
                parent_elem, "literal", {"class": "False", "value": "false"}
            )
        else:
            self.create_xml_element(parent_elem, "var", {"name": token_value})


def print_and_exit(msg, exit_code, output=sys.stdout):
    """Print message and exit program with specified code"""
    try:
        print(msg, file=output)
        exit(exit_code)
    except IOError:
        exit(ERROR_SCRIPT_OUTPUT)


def parse_args():
    """Parse arguments"""
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
    """Validate string token for lexical correctness"""
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
    """Parse SOL25 source code from stdin"""
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
        xml_tree = ASTConverter().generate_xml(ast, comments)

        return xml_tree
    except UnexpectedCharacters as e:
        print_and_exit(f"Lexical error: {e}", ERROR_LEXICAL, sys.stderr)
    except UnexpectedToken as e:
        print_and_exit(f"Syntax error: {e}", ERROR_SYNTAX, sys.stderr)
    except Exception as e:
        print_and_exit(f"Internal error: {e}", ERROR_INTERNAL, sys.stderr)


def prettify(xml_tree):
    """Format XML tree with indentations"""
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

