"""
Goal: Work with decorators
Task: Program a function that deals with its decorators and calls
"""

import collections
from functools import wraps

my_counter = collections.Counter()

def log_and_count(key='f3', counts=my_counter):
    def log_and_count_dec(func):
        @wraps(func)
        def inner(*args, **kwargs):
            counts[key] = counts.get(key, 0) + 1
            print('Called {} with : {} and {}'.format(func.__name__, args, kwargs))
        return inner
    return log_and_count_dec

@log_and_count(key = 'basic functions', counts = my_counter)
def f1(a, b=2):
    return a ** b

@log_and_count(key = 'basic functions', counts = my_counter)
def f2(a, b=3):
    return a ** 2 + b

@log_and_count(counts = my_counter)
def f3(a, b=5):
    return a ** 3 - b

f1(2)
f2(2, b=4)
f1(a=2, b=4)
f2(4)
f2(5)
f3(5)
f3(5,4)

print(my_counter)
