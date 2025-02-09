#!/usr/bin/env python3

"""
Goal: Work with strings
"""

def she_says_he_says(she_says):
    
    """
    Task: Write function that transforms onw string to another strictly by pattern in example
    
    >>> she_says_he_says('ma rymu')
    'umiram'
    """

    phonetic_she_says = she_says[::-1]                              
    compact = phonetic_she_says.replace(" ", "")                      
    he_says = compact.replace("y", "i")    
    
    return he_says

def solfege(title_hymn):
    """
    Task:
        Partitions the input string to (an optional) title, ': ', and the hymn,
        takes a sublist starting from the first string, skipping always two 
        other strings, and ending 3 strings from the end, returns the result 
        as a string with ', ' as a separator

    >>> solfege('Hymn of St. John: Ut queant laxis re sonare fibris mi ra gestorum fa muli tuorum sol ve polluti la bii reatum Sancte Iohannes')
    'Ut, re, mi, fa, sol, la'

    >>> solfege('Ut queant laxis re sonare fibris mi ra gestorum fa muli tuorum sol ve polluti la bii reatum Sancte Iohannes')
    'Ut, re, mi, fa, sol, la'
    """

    if ':' in title_hymn:
        possible_title, hymn = title_hymn.split(": ")
    else:
        hymn = title_hymn                                                

    hymn_list = hymn .split(" ")                                         
    skip2 = hymn_list[:-3:3]                                             
    skip2_str = ', '.join(skip2)                                         

    return skip2_str


if __name__ == "__main__":
    import doctest
    doctest.testmod()
    pass
