#include <stdio.h>
#include "variadic_functions.h"

/**
 * print_all - prints anything
 * @format: list of types of arguments
 *
 * Return: nothing
 */
void print_all(const char * const format, ...)
{
	va_list args;
	unsigned int i;
	char *s, *sep;

	sep = "";
	i = 0;
	va_start(args, format);
	while (format != NULL && format[i] != '\0')
	{
		switch (format[i])
		{
		case 'c':
			printf("%s%c", sep, va_arg(args, int));
			sep = ", ";
			break;
		case 'i':
			printf("%s%d", sep, va_arg(args, int));
			sep = ", ";
			break;
		case 'f':
			printf("%s%f", sep, va_arg(args, double));
			sep = ", ";
			break;
		case 's':
			s = va_arg(args, char *);
			if (s == NULL)
				s = "(nil)";
			printf("%s%s", sep, s);
			sep = ", ";
			break;
		}
		i++;
	}
	va_end(args);
	printf("\n");
}