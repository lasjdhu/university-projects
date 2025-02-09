# @brief Inicializace interpretu
# @file interpret.py
# @author Dmitrii Ivanushkin xivanu00

import sys
import argparse
import xml.etree.ElementTree as ET
from Program import Program

# @brief Vyhledavani programu v jazyce IPPcode23 v XML souboru a parser vstupu
class Interpret():
	# @brief Init
	# @param object self
	def __init__(self):
		self.program = None

		self.parse_args()
		self.work_w_args()
		self.load_inputs()
		self.load_program()
		self.check_program()

		program = Program(self.program)
		sys.exit(0) # uspech
	
	# @brief Parser vstupu a overeni spravnosti volani interpret.py
	# @param object self
	def parse_args(self):
		parser = argparse.ArgumentParser(add_help=False)
		parser.add_argument("--help", action="store_true",
												help="Nápověda")
		parser.add_argument("--source", metavar="FILE",
												help="Vstupní soubor s XML reprezentací zdrojového kódu")
		parser.add_argument("--input", metavar="FILE",
												help="Soubor se vstupy pro samotnou interpretaci zadaného zdrojového kódu")
		self.args = parser.parse_args()

		if self.args.help:
			if len(sys.argv) > 2:
				print("ERROR: Jenom [--help] povolen pri volani [--help]", file=sys.stderr)
				sys.exit(10)
			parser.print_help()
			sys.exit(0)
	
	# @brief Nahrada chybejicich vstupu z stdin
	# @param object self
	def work_w_args(self):
		if self.args.source and self.args.input:
			self.src_xml = self.args.source
			self.src_input = self.args.input
		elif self.args.source and not self.args.input:
			self.src_xml = self.args.source
			self.src_input = sys.stdin
		elif not self.args.source and self.args.input:
			self.src_xml = sys.stdin
			self.src_input = self.args.input 
		else:
			print("ERROR: Napiste nejmene jeden [--source] or [--input]", file=sys.stderr)
			sys.exit(10)
	
	# @brief Vstup pro specialni operace (napr. READ)
	# @param object self
	def load_inputs(self):
		if isinstance(self.src_input, str):
			try:
				with open(self.src_input) as f: self.input = f.readlines()
			except FileNotFoundError:
				print(f"ERROR: Soubor '{self.src_input}' nebyl nalezen", file=sys.stderr)
				sys.exit(11)
		else:
			self.input = self.src_input.readlines()
	
	# @brief Vyhledavani programu v jazyve IPPcode23 v XML souboru
	# @param object self
	def load_program(self):
		try:
			tree = ET.parse(self.src_xml)
			self.program = tree.getroot()
		except ET.ParseError as e:
			print(f"ERROR: Nepodarilo se nacist XML: {e}", file=sys.stderr)
			sys.exit(31)
	
	# @brief Overeni spravnosti struktury XML stromu
	# @param object self
	def check_program(self):
		if self.program.tag != "program":
			print("ERROR: Neni <program> tag", file=sys.stderr)
			sys.exit(32)
		if ("name" not in self.program.attrib) and ("description" not in self.program.attrib):
			if  "language" not in self.program.attrib:
				print("ERROR: Chybi atribut v <program>", file=sys.stderr)
				sys.exit(32)
		if self.program.attrib["language"].lower() != "ippcode23":
			print("ERROR: Jazyk neni IPPcode23", file=sys.stderr)
			sys.exit(32)

		for instruction in self.program:
			if instruction.tag != "instruction":
				print("ERROR: Neni <instrukce> tag", file=sys.stderr)
				sys.exit(32)
			if ("order" not in instruction.attrib) or ("opcode" not in instruction.attrib):
				print("ERROR: Chybi atribut v <instruction>", file=sys.stderr)
				sys.exit(32)
			
			cnt = 0
			for arg in instruction:
				cnt+=1
				if arg.tag != f"arg{cnt}":
					print("ERROR: Nespravne jmeno tagu", file=sys.stderr)
					sys.exit(32)
				if ("type" not in arg.attrib):
					print("ERROR: Chybi atribut v <arg$>", file=sys.stderr)
					sys.exit(32)

# Init
if __name__ == "__main__":
	interpret = Interpret()
