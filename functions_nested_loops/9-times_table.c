#include "main.h"

/**
 * times_table - prints the 9 times table, starting with 0
 *
 * Return: void
 */
void times_table(void)
{
	int i, j, n;

	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 10; j++)
		{
			n = i * j;

			if (j != 0)
			{
				_putchar(',');
				_putchar(' ');
			}

			if (n >= 10)
				_putchar(n / 10 + '0');

			_putchar(n % 10 + '0');
		}

		_putchar('\n');
	}
}
