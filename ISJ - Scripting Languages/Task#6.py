"""
Goal: Work with OOP
Task: Program a class that operates with polynoms
"""

class Polynomial:
    
    polynom = []
    
    def __init__(self, *listnum, **words):
        self.polynom = ["0"]                  # marking the first element so we won't get problem with mutable types of different vars
        sign = False
        if listnum: 
            if isinstance(listnum[0], list):  # if list
                self.polynom = listnum[0]
            else:                             # if set
                for el in listnum:
                    self.polynom.append(el) 
                del self.polynom[0]           # marking polynom
        if words:                             # if has keys and values
            for key, value in words.items():
                index = int(key[-1])          # getting index of value from key
                if index == 0:
                    sign = True               # we've got first symbol so we want to reset polynom
                for i in range(len(self.polynom), index + 1):
                    self.polynom.append(0)    # writing 0 in spaces between values
                self.polynom[index] = int(value)
            if not sign:                      #reseting polynom 
                self.polynom[0] = 0
        while self.polynom[-1] == 0:          # deleting all 0 at the end of a list because we don't need them
            del self.polynom[-1]
    
    def __repr__(self):
        return str(self.polynom)              # for representation purposes
    
    def __str__(self):
        my_string = ""                                   # string to return
        reverse_string = ""
        for key, value in enumerate(self.polynom):
            if value == 0:                               # if we have 0 we move to next iteration
                continue
            if value < 0:
                reverse_string = " - " + str(abs(value)) # abs because in string we will reverse we will need only value
            if value > 0:
                reverse_string = " + " + str(value)  
            if key == 1:
                reverse_string = reverse_string + "x"
            if key > 1 and key != (len(self.polynom) - 1):
                reverse_string = reverse_string + "x^" + str(key)
            if key > 1 and key == (len(self.polynom) - 1):
                reverse_string = str(value) + "x^" + str(key)
            my_string = reverse_string + my_string
        return my_string
    
    def __eq__(self, __o) -> bool:                     # we will have second object here
        if len(self.polynom) == len(__o.polynom):      # checking for the same length
            for key, value in enumerate(self.polynom):
                if value != __o.polynom[key]:          # checking for the same value
                    return False
            return True
        else:
            return False
    
    def __add__(self, __o):                            
        plus = []
        if len(self.polynom) >= len(__o.polynom):      # filling list where is more values
            for key, value in enumerate(self.polynom):
                plus.append(value)
            for key, value in enumerate(__o.polynom):
                plus[key] += value                     # adding to it values from the second list
            plus = Polynomial(plus)
            return plus
        if len(self.polynom) < len(__o.polynom):
            for key, value in enumerate(__o.polynom):
                plus.append(value)
            for key, value in enumerate(self.polynom):
                plus[key] += value
            plus = Polynomial(plus)
            return plus