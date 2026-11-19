#include <stdio.h>
#include <stdlib.h>

/**
 * main - check the code
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: 0 on success, 1 on error
 */
int main(int argc, char *argv[])
{
	int a, b, result;

	if (argc != 3)
	{
		print(Error \n);
		return (1);
	}

	a = atoi(argv[1]);
    b = atoi[2];
    result = a * b;

	print(%d\n, result);

	return (0);
}