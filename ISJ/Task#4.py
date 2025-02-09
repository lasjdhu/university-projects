#!/usr/bin/env python3

"""
Goal: Work with permutations and dictionaries
"""

from itertools import permutations

def all_permutations_substrings(a_str):
    #Generates all permutations of all substrings of the input string

    return set(
        ''.join(item)
        for length in range(len(a_str)+1)
        for item in permutations(a_str, length))
        
def match_permutations_substrings(string, words):
    
    """
    Task:
        Generates all permutations of all substrings of the input string and
        returns a set of input words that match one of the permutations.

    >>> match_permutations_substrings('okna', ['a', 'z', 'v', 'o', 'k', 'ok', 'ano', 'no', 'hlava', 'oko', 'noky', 'nok', 'on', 'ona', 'ony']) == {'ona', 'a', 'ok', 'o', 'nok', 'no', 'ano', 'on', 'k'}
    True

    >>> match_permutations_substrings('opak', ['ok', 'pak', 'pako', 'ano', 'noha', 'oka', 'kap', 'kopa', 'kopat', 'ona', 'okap']) == {'kopa', 'kap', 'pako', 'ok', 'pak', 'okap', 'oka' }
    True

    """
    
    perms = all_permutations_substrings(string)
    res = {x for x in perms if(x in words)}

    return res                                      


def uniq_srt(it):
    
    """
    Task:
        Returns the input sequence unified and sorted (according to the values)

    >>> uniq_srt([3, 3, 5, 3, 4, 2, 4])
    [2, 3, 4, 5]

    >>> uniq_srt('abrakadabra')
    ['a', 'b', 'd', 'k', 'r']

    """

    uniq_srt = sorted(it)
    new_uniq_srt = list(dict.fromkeys(uniq_srt))

    return new_uniq_srt                                 

def uniq_orig_order(it):
    
    """
    Task:
        Returns the input sequence, items ordered by the order of their
        first appearance

    >>> uniq_orig_order([3, 3, 5, 3, 4, 2, 4])
    [3, 5, 4, 2]

    >>> uniq_orig_order('abrakadabra')
    ['a', 'b', 'r', 'k', 'd']

    """
    
    uniq_orig_order = list(it)
    new_uniq_orig_order = list(dict.fromkeys(uniq_orig_order))

    return new_uniq_orig_order                                      


if __name__ == "__main__":
    import doctest
    doctest.testmod()
