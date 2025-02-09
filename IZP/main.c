/*
 * main.c
 * Theme: Password check
 * Author: Dmitrii Ivanushkin, october 2021
 */
#include <stdio.h>
#include <stdlib.h>

#define max_len 100             

// I can't use some libraries in this project, so I wrote help functions
int strLen (char str[max_len]){
    int i = 0;                  

    while (str[i] != '\0') 
        i++; 

    return i;
}

// Help function to check if level and param are integers 
// and to get their value by ptr
int convertStr (int* arg, char* str){
    int len = strLen(str);

    for (int i = 0; i < len; i++)
    {
        if ((str[i] < 48) || (str[i] > 57))
        {
            fprintf(stderr, "You entered the wrong argument\n");
            return -1;
        }
    }
    
    *arg = atoi(str);
    return 0;
}

// Help functions to compare two strings
int cmprStr (char* str1, char* str2){
    if (strLen(str1) != strLen(str2))
        return 0;

    int len = strLen(str1);
    for (int i = 0; i < len; i++)
        if (str1[i] != str2[i])
            return 0;

    return 1;
}

// Help fuction to find diffs in passwords for statistics
void difSym (char* asciit, char pswd[max_len]){
    int len = strLen(pswd);

    for (int i = 0; i < len; i++)
        *(asciit + pswd[i]) = 1;
}

// Check if there is any lower case letter in the string
int lowerCase (char str[max_len]) {
    for (int i = 0; i < max_len; i++)
    {
        if ((str[i] >= 'a') && (str[i] <= 'z'))
            return 1;
        if (str[i] == '\0') 
            break;
    }

    return 0;
}

// Check if there is any upper case letter in the string
int upperCase (char str[max_len]) {
    for (int i = 0; i < max_len; i++)
    {
        if ((str[i] >= 'A') && (str[i] <= 'Z'))
            return 1;
        if (str[i] == '\0')
            break;
    }

    return 0;
}

// Check if there is any digit in the string
int numSym (char str[max_len]) {
    for (int i = 0; i < max_len; i++)
    {
        if ((str[i] >= '0') && (str[i] <= '9'))
            return 1;
        if (str[i] == '\0')
            break;
    }

    return 0;
}

// Check if there is any special symbol in the string
int specSym (char str[max_len]) {
    for (int i = 0; i < max_len; i++)
    {
        if (((str[i] > 31) && (str[i] < 48)) || 
            ((str[i] > 57) && (str[i] < 65)) || 
            ((str[i] > 90) && (str[i] < 97)) || 
            ((str[i] > 122) && (str[i] < 127)))
            return 1;
        if (str[i] == '\0')
            break;
    }

    return 0;
}

int lvl_1 (char str[max_len]) {
    if (lowerCase(str) && upperCase(str))
        return 1;
    
    return 0;
}

int lvl_2 (int p, char str[max_len]) {
    // Level 2 will be passed only if level 1 is already passed
    if (!(lvl_1(str)))
        return 0;

    switch (p)
    {
    case 1:
        // Because level 1 is already passed
        return 1;
        break;
    case 2:
        // Because level 1 is already passed
        return 1;
        break;
    case 3:
        if (numSym(str) || specSym(str))
            return 1;
        break;
    default:
        if (numSym(str) && specSym(str))
            return 1;
        break;
    }

    return 0;
}

int lvl_3 (int p, char str[max_len]) {
    // Level 3 will be passed only if level 2 is already passed
    if (!(lvl_2(p, str)))
        return 0;

    int cnt = 1;

    for (int i = 1; str[i] != '\0'; i++)
    {
        if (str[i] == str[i-1])
            cnt++;
        else
            // Start position
            cnt = 1;
        
        // Found sequence of p symbols
        if (cnt == p)
            return 0;
    }
     
    return 1;
}

int lvl_4 (int p, char* str) {
    // Level 4 will be passed only if level 3 is already passed
    if (!(lvl_3(p, str)))
        return 0;

    int cnt = 0;

    // The element after first symbol in substrings
    int iFori;
    int jForj;

    int len = strLen(str);

    for (int i = 0; i < len - 1; i++)
    {
        for (int j = i + 1; j <= len - 1; j++)
        {
            if (str[i] == str[j])
            {
                cnt++;

                // Check the neighbor of the element in substrings
                iFori = i + 1;
                jForj = j + 1;

                for (int k = 1; k < p; k++)
                {
                    // The neighbors are the same symbol
                    if (str[iFori] == str[jForj])
                    {
                        cnt++;

                        iFori++;
                        jForj++;
                    }

                    // Count of symbols in substrings is p
                    if (cnt == p)
                        return 0;
                }
            }
            // Set to zero if substring is not found and go on
            else 
                cnt = 0;
        }
    }
    
    // Check sums and close function if substring is not found
    if (cnt != p)
        return 1; 

    return 0;
}

// Reading passwords from stdout
int passInput (char* pswd) {
    char c;
    int i = 0;

    while ( (c = getchar()) != '\n')
    {
        //Password shouldn't be longer than 100
        if (i > max_len - 1)
            return -1;
        
        if (c == EOF)
            return -2;

        //For each symbol in password we write character 'c'
        *(pswd + i) = c;
        i++; 
    }
    
    return 0;
}

// Check passwords with filters we got
int checkPw (int lvl, int p, int stats) {
    // Buffer for passwords
    char pswd[max_len];

    int codeOfExit;

    int LENGTH = 0; 
    int MIN = max_len;
    int NCHARS = 0;
    
    // Buffer for ascii
    char asciit[127];

    // Create an ascii array filled with zeros
    for (int i = 0; i < 127; i++)
        asciit[i] = 0;

    int cnt = 0;
    while (1)
    {
        // Refresh buffer every string
        for (int i = 0; i < max_len; i++)
            pswd[i] = 0;

        codeOfExit = passInput(pswd);

        if (codeOfExit == -1)
        {
            fprintf(stderr, "Password is too long\n");
            return -1;
        }
        // EoF
        if (codeOfExit == -2)
            break;

        // Checks by level for each password in file
        switch (lvl)
        {
        case 1:
            if (lvl_1(pswd))
                printf("%s\n", pswd);
            break;
        case 2:
            if (lvl_2(p, pswd))
                printf("%s\n", pswd);
            break;
        case 3:
            if (lvl_3(p, pswd))
                printf("%s\n", pswd);
            break;
        case 4:
            if (lvl_4(p, pswd))
                printf("%s\n", pswd);
            break;
        default:
            break;
        }

        // Finding minimum
        if (MIN > strLen(pswd))
            MIN = strLen(pswd);

        // Finding length
        LENGTH +=strLen(pswd);

        // Filling ASCII table (if stats flag is enabled for convenience)
        if (stats == 1)
            difSym(asciit, pswd);

        cnt++;
    }
    
    // Statistics print
    if (stats == 1)
    {
        double AVG = (double)LENGTH / cnt;

        for (int i = 0; i < 127; i++)
            if (asciit[i] != 0)
                NCHARS++;
        
        printf("Statistics: \n");
        printf("Different symbols: %d\n", NCHARS);
        printf("Min length of password: %d\n", MIN);
        printf("Averege length of password: %.1f", AVG);
    }

    return 0;
}

int main (int argc, char* argv[]) {
    int lvl = 0;
    int p = 0;
    int stats = 0;

    // User should enter at least level and parametr
    if (argc != 3)
    {
        fprintf(stderr, "You entered too few arguments\n");
        return -1;
    }

    for (int i = 1; i < argc; i++)
    {
        switch (i)
        {
        case 1:
            // On the first place should be security level
            convertStr(&lvl, argv[i]);
            if ((lvl > 4) || (lvl < 1))
            {
                fprintf(stderr, "Level number should not be less than 1 and greater than 4\n");
                return -1;
            }
            break;
        case 2:
            // On the second place should be security parameter
            convertStr(&p, argv[i]);
            if (p < 1)
            {
                fprintf(stderr, "Parameter number should not be less than 1\n");
                return -1;
            }
            break;
        case 3:
            if (cmprStr(argv[i], "--stats"))
                stats = 1;
            if (!(cmprStr(argv[i], "--stats")))
            {
                fprintf(stderr, "Statistics is called by '--stats' command\n");
                return -1;
            }
            break;
        default:
            break;
        }
    }

    if (checkPw(lvl, p, stats) != 0)
        return -1;

    return 0;
}
