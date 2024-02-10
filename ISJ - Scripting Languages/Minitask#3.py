"""
Goal: Work with iterators
Task: Iterate through them (f is a general iterator, for example a file)
"""

f = open('students.txt')
it = f.readlines()
header = it.pop(0)
for line in it:
    line = line.rstrip()
    print(line)
