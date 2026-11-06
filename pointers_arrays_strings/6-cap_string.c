#include "main.h"

/**
 * cap_string - capitalizes all words of a string
 * @str: the string to modify
 *
 * Return: a pointer to the modified string
 */
char *cap_string(char *str)
{
	int i = 0;
	int j;
	int new_word = 1;
	char sep[] = " \t\n,;.!?\"(){}";

	while (str[i] != '\0')
	{
		if (new_word && str[i] >= 'a' && str[i] <= 'z')
			str[i] -= 32;

		new_word = 0;
		for (j = 0; sep[j] != '\0'; j++)
		{
			if (str[i] == sep[j])
				new_word = 1;
		}
		i++;
	}

	return (str);
}
