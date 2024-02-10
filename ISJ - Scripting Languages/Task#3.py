#!/usr/bin/env python3

"""
Goal: Work with statements and lists
"""

def first_odd_or_even(numbers):
    """
    Task:
        Returns 0 if there is the same number of even numbers and odd numbers
        in the input list of ints, or there are only odd or only even numbers.
        Returns the first odd number in the input list if the list has more even
        numbers.
        Returns the first even number in the input list if the list has more odd 
        numbers.

    >>> first_odd_or_even([2,4,2,3,6])
    3
    >>> first_odd_or_even([3,5,4])
    4
    >>> first_odd_or_even([2,4,3,5])
    0
    >>> first_odd_or_even([2,4])
    0
    >>> first_odd_or_even([3])
    0
    """

    i = k = 0

    for num in numbers:
        if (num % 2) == 0:
            i+=1
        else:
            k+=1

    if (i == k) or (i == 0 and k != 0) or (i != 0 and k == 0):
        return 0
   
    if i > k:
        for num in numbers:
            if (num % 2) != 0:
                odd = num
                break
        return odd
    
    if i < k:
        for num in numbers:
            if (num % 2) == 0:
                even = num
                break
        return even



def to_pilot_alpha(word):
    """
    Task: Decode string with list

    >>> to_pilot_alpha('Smrz')
    ['Sierra', 'Mike', 'Romeo', 'Zulu']
    """

    pilot_alpha = ['Alfa', 'Bravo', 'Charlie', 'Delta', 'Echo', 'Foxtrot',
        'Golf', 'Hotel', 'India', 'Juliett', 'Kilo', 'Lima', 'Mike',
        'November', 'Oscar', 'Papa', 'Quebec', 'Romeo', 'Sierra', 'Tango',
        'Uniform', 'Victor', 'Whiskey', 'Xray', 'Yankee', 'Zulu']

    pilot_alpha_list = []

    for i in range(len(word)):
        for k in range(len(pilot_alpha)):
            if pilot_alpha[k].lower().startswith(word[i].lower()):
                pilot_alpha_list.append(pilot_alpha[k])
     
    return pilot_alpha_list


if __name__ == "__main__":
    import doctest
    doctest.testmod()
