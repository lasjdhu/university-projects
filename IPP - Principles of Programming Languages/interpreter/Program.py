# @brief Trida Program
# @file Program.py
# @author Dmitrii Ivanushkin xivanu00

from Instruction import Instruction
from Frame import Frame
from Operations import *

# @brief Trida pro spravu vsech instrukci po dobu behu programu
class Program:
	# @brief Init
	# @param object self
	# @param object program (XML root stromu)
	def __init__(self, program):
		self.program = program
		self.frame = Frame()
		self.labels = {}
		self.parse_labels()
		self.parse_instructions()
	
	# @brief Finds all labels in code
	def parse_labels(self):
		i = 0
		for element in self.program:
			i+=1
			instruction = Instruction()
			instruction.push(element)
			# @brief LABEL <label>
			if instruction.opcode == "LABEL":
				self.labels[instruction.arg1["text"]] = i
	
	# @brief Iterace pres vse instrukce
	# @param object self
	def parse_instructions(self):
		i = 0
		while i < len(self.program):
			element = self.program[i]
			instruction = Instruction()
			instruction.push(element)
			self.find_frames(instruction)
			self.choose_op(instruction)
			
			# @brief JUMP <label>
			if instruction.opcode == "JUMP":
				i = self.labels[instruction.arg1["text"]]
			else:
				i+=1
	
	# @brief Rozdeleni textu uvnitr argumentu na ramec a hodnotu
	# @param object self
	# @param object instruction
	def find_frames(self, instruction):
		if instruction.arg1["text"] != None and "@" in instruction.arg1["text"]:
			instruction.arg1["frame"], instruction.arg1["text"] = instruction.arg1["text"].split("@")

		if instruction.arg2["text"] != None and "@" in instruction.arg2["text"]:
			instruction.arg2["frame"], instruction.arg2["text"] = instruction.arg2["text"].split("@")

		if instruction.arg3["text"] != None and "@" in instruction.arg3["text"]:
			instruction.arg3["frame"], instruction.arg3["text"] = instruction.arg3["text"].split("@")
	
	# @brief Rozcestnik operaci, pouziva tridy z 'Operations.py'
	# @param object self
	# @param object instruction
	def choose_op(self, instruction):
		if instruction.opcode == "MOVE":
			move = Move(self.frame, instruction)
		if instruction.opcode == "CREATEFRAME":
			createframe = Createframe(self.frame)
		if instruction.opcode == "PUSHFRAME":
			pushframe = Pushframe(self.frame)
		if instruction.opcode == "POPFRAME":
			popframe = Popframe(self.frame)
		if instruction.opcode == "DEFVAR":
			defvar = Defvar(self.frame, instruction)
		if instruction.opcode == "WRITE":
			write = Write(self.frame, instruction)
		if instruction.opcode == "DPRINT":
			dprint = Dprint(instruction)
		if instruction.opcode == "BREAK":
			break1 = Break(self.frame, instruction)
			