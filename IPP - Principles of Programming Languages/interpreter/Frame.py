# @brief Trida Frame
# @file Frame.py
# @author Dmitrii Ivanushkin xivanu00

import sys

# @brief Trida pro logovani stavu ramce a praci s nimi
class Frame:
	# @brief Init
	# @param object self
	def __init__(self):
		self.GF = {}
		self.TF = None
		self.stack = []
	
	# @brief Definuje TF
	# @param object self
	def create(self):
		self.TF = {}
	
	# @brief Getter
	# @param object self
	# @param string frame
	def get(self, frame):
		if frame == "GF": return self.GF
		if frame == "LF": 
			if len(self.stack) > 0:
				return self.stack[-1]
			else:
				return None
		if frame == "TF": return self.TF
	
	# @brief Definuje promennu
	# @param object self
	# @param string f_to (frame)
	# @param string var
	def defI(self, f_to, var):
		if self.get(f_to) == None:
			print("ERROR: Ramec neexistije", file=sys.stderr)
			sys.exit(55)
		if f_to == "GF":
			self.GF[var] = None
		elif f_to == "LF":
			self.stack[-1][var] = None
		elif f_to == "TF":
			self.TF[var] = None
		elif var in self.get(f_to):
			print("ERROR: Redefinice promenne", file=sys.stderr)
			sys.exit(52)	

	# @brief Setter
	# @param object self
	# @param string f_to (frame)
	# @param string var
	# @param any value
	def set(self, f_to, var, value):
		if self.get(f_to) == None:
			print("ERROR: Ramec neexistije", file=sys.stderr)
			sys.exit(55)
		if var in self.GF and f_to == "GF":
			self.GF[var] = value
		elif var in self.stack and f_to == "LF":
			self.stack[-1] = value
		elif var in self.TF and f_to == "TF" and self.TF != None:
			self.TF[var] = value
		elif var not in self.get(f_to):
			print("ERROR: Pristup k neexistujici promenne", file=sys.stderr)
			sys.exit(54)

	# @brief push
	# @param object self
	def push(self):
		if self.TF == None:
			print("ERROR: Ramec neexistije", file=sys.stderr)
			sys.exit(55)
		self.stack.append(self.TF)
	
	# @brief pop
	# @param object self
	def pop(self):
		if len(self.stack) == 0:
			print("ERROR: Ramec neexistije", file=sys.stderr)
			sys.exit(55)
		self.stack.pop()
		self.create()
	