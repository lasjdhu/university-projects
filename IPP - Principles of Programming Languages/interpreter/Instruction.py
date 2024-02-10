# @brief Trida Instruction
# @file Instruction.py
# @author Dmitrii Ivanushkin xivanu00

import sys

# @brief Trida pro chraneni opcode a argumentu pro soucasnou instrukci
class Instruction:
	# @brief Init
	# @param object self
	def __init__(self):
		self.opcode = None
		self.arg1 = {"type": None, "frame": None, "text": None}
		self.arg2 = {"type": None, "frame": None, "text": None}
		self.arg3 = {"type": None, "frame": None, "text": None}

	# @brief Naplneni instrukci a overeni spravnost
	# @param object self
	# @param object instruction
	def push(self, instruction):
		self.opcode = instruction.attrib["opcode"]

		one_arg = ["DEFVAR", "POPS", "PUSHS", "WRITE", "EXIT", "DPRINT", "CALL", "LABEL", "JUMP"]
		two_args = ["READ", "MOVE", "INT2CHAR", "STRLEN", "TYPE"]
		three_args = ["ADD", "SUB", "MUL", "IDIV","LT", "GT", "EQ", "AND", "OR",
		"NOT", "STRI2INT", "CONCAT", "GETCHAR", "SETCHAR", "JUMPIFEQ", "JUMPIFNEQ"]
		no_args = ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]
	
		if instruction.attrib["opcode"].upper() in one_arg:
			if len(instruction) != 1:
				print("ERROR: Nespravne cislo argumentu", file=sys.stderr)
				sys.exit(32)
			self.arg1 = {"type": instruction[0].attrib["type"], "text": instruction[0].text, "frame": ""}
		elif instruction.attrib["opcode"].upper() in two_args:
			if len(instruction) != 2:
				print("ERROR: Nespravne cislo argumentu", file=sys.stderr)
				sys.exit(32)
			self.arg1 = {"type": instruction[0].attrib["type"], "text": instruction[0].text, "frame": ""}
			self.arg2 = {"type": instruction[1].attrib["type"], "text": instruction[1].text, "frame": ""}
		elif instruction.attrib["opcode"].upper() in three_args:
			if len(instruction) != 3:
				print("ERROR: Nespravne cislo argumentu", file=sys.stderr)
				sys.exit(32)
			self.arg1 = {"type": instruction[0].attrib["type"], "text": instruction[0].text, "frame": ""}
			self.arg2 = {"type": instruction[1].attrib["type"], "text": instruction[1].text, "frame": ""}
			self.arg3 = {"type": instruction[2].attrib["type"], "text": instruction[2].text, "frame": ""}
		elif instruction.attrib["opcode"].upper() in no_args:
			if len(instruction) != 0:
				print("ERROR: Nespravne cislo argumentu", file=sys.stderr)
				sys.exit(32)
		else:
			print("ERROR: Nespravny opcode", file=sys.stderr)
			sys.exit(32)
	