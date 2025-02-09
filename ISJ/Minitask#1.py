"""
Goal: Work with regex
Task: Print tags without brackets
"""

import re

text = '</note> and <b>foo</b> and <i>so on</i> and 1 < 2'
print(re.findall(r'(?<=<).*?(?=>)',text))
