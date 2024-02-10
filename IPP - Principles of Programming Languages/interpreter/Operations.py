# @brief Soubor vsech operaci
# @file Operations.py
# @author Dmitrii Ivanushkin xivanu00

import sys

# @brief MOVE <var> <symb>
class Move:
	# @brief Prirazeni hodnoty do promenne
	# @param object self
	# @param object frame (globalni objekt)
	# @param object instruction
	def __init__(self, frame, instruction):
		type_var = instruction.arg1["type"]
		type_symb = instruction.arg2["type"]
		f_var = instruction.arg1["frame"]
		f_symb = instruction.arg2["frame"]
		var = instruction.arg1["text"]
		symb = instruction.arg2["text"]

		if type_var == "var" and not type_symb == "var":
			frame.set(f_var, var, symb)
		elif type_var == "var" and type_symb == "var":
			if symb in frame.get(f_symb) or f_symb == "": 
				frame.set(f_var, var, symb)
			else:
				print("ERROR: Ramec neexistuje", file=sys.stderr)
				sys.exit(55)
		else:
			print("ERROR: Vnitrni", file=sys.stderr)
			sys.exit(52)

# @brief CREATEFRAME
class Createframe:
  # @brief Vytvor novy docasny ramec
	# @param object self
	# @param object frame (globalni objekt)
	def __init__(self, frame):
		frame.create()

# @brief PUSHFRAME
class Pushframe:
  # @brief Presun docasneho ramce na zasobnik ramcu
	# @param object self
	# @param object frame (globalni objekt)
	def __init__(self, frame):
		frame.push()

# @brief POPFRAME
class Popframe:
  # @brief Presun aktualniho ramce do docasneho
	# @param object self
	# @param object frame (globalni objekt)
	def __init__(self, frame):
		frame.pop()

# @brief DEFVAR <var>
class Defvar:
  # @brief Definuj novou promennu v ramci
	# @param object self
  # @param object frame (globalni objekt)
	# @param object instruction
	def __init__(self, frame, instruction):
		f_var = instruction.arg1["frame"]
		var = instruction.arg1["text"]

		frame.defI(f_var, var)

# @brief CALL <label>
class Call:
	# @brief Skok na navesti s podporou navratu
	# @param object self
	def __init__(self):
		pass

# @brief RETURN
class Return:
	# @brief Navrat na pozici ulozenou instrukci CALL
	# @param object self
	def __init__(self):
		pass

# @brief WRITE
class Write:
	# @brief Vupis hodnoty na standardni vystup
	# @param object self
	# @param object frame
	# @param object instruction
	def __init__(self, frame, instruction):
		f_var = instruction.arg1["frame"]
		var = instruction.arg1["text"]

		print(frame.get(f_var)[var])

# @brief DPRINT <symb>
class Dprint:
	# @brief Vypis hodnoty na stderr
	# @param object self
	# @param object instruction
	def __init__(self, instruction):
		symb = instruction.arg1["text"]
		print(symb)

# @brief BREAK
class Break:
	# @brief Vypis stavu interpretu na stderr
	# @param object self
	# @param object frame
	# @param object instruction
	def __init__(self, frame, instruction):
		print("--------------------------------")
		print("Stav interpretu na danou chvili:")
		print(f"GF: {frame.get('GF')}")
		print(f"LF: {frame.get('LF')}")
		print(f"TF: {frame.get('TF')}")
		print(f"Last instruction: {instruction.opcode}")
		print("--------------------------------")