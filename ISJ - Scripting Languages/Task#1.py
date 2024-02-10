#!/usr/bin/env python3

"""
Goal: Work with regex
Task: write dunction that changes camel case to snake case
"""

import re

def camel_to_snake_case(name):

    """
        >>> camel_to_snake_case('camelCaseNameAllowed')
        'camel_case_name_allowed'
        >>> camel_to_snake_case('longVATNumber')
        'long_vat_number'
    """

    inbetween = re.compile(r'''
                            (
                             (?<!^)[A-Z](?=[a-z])
                             |
                             (?<!^)(?<=[a-z])[A-Z]
                            )
                            ''', re.VERBOSE)
    return inbetween.sub(r'_\1', name).lower()

if __name__ == "__main__":
    import doctest
    doctest.testmod()
