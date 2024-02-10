/*
 * Palindrom checker
 * @file main.c
 * @author Dmitrii Ivanuhskin (xivanu00)
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>		// exit()
#include <string.h>		// strlen()
#include <ctype.h>		// tolower()

#define MAX_LENTGH 15
#define USER_ERROR -1

/*
 * Gets user input and checks it for errors
 * @param *str user input
 */
void getUserInput(char *str) {
	printf("Prosím zadejte váš řetězec (max 15 znaků): ");
	scanf("%s", str);
	if (strlen(str) > MAX_LENTGH) {
		fprintf(stderr, "Zadaný řetězec je delší než povolený maximum znaků (%d)\n", MAX_LENTGH);
		exit(USER_ERROR);
	}
}

/*
 * Checks if given string is a palindrom or not and prints result to a user
 * @param *str user input
 */
void checkIfPalindrom(char *str) {
	int len = strlen(str);
	bool isPalindrom = true;
	bool insensitiveFlag = false;
	int index = 0;

	for (int i = 0; i < len / 2; i++) {
		if (str[i] != str[len - i - 1]) {
			if (tolower(str[i]) != tolower(str[len - i - 1])) {
				isPalindrom = false;
				index = i;
				break;
			} else {
				insensitiveFlag = true;
			}
		}
	}

	if (isPalindrom && !insensitiveFlag)
		printf("Zadané slovo JE symetrické\n");
	else if (isPalindrom && insensitiveFlag)
		printf("Zadané slovo JE symetrické (case-insensitive)\n");
	else
		printf("Zadané slovo NENÍ symetrické, první rozdíl je na indexech %d a %d\n", index, len - index - 1);
}

/*
 * Main function
 */
int main(void)
{
	char input[MAX_LENTGH];

	getUserInput(input);
	checkIfPalindrom(input);
	
	return 0;
}
