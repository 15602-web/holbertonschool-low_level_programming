#include "main.h"
/**
 * main - check the code
 *function that changes all lowercase letters of a string to uppercase.
 * Return: Always 0.
 */

char *string_toupper(char *);
{

	int i = 0;

	while (str[i] != '\0')

		{
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] -= 32;
		i++;
		}

	return(str);
}
