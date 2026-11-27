#ifndef MAIN_H
#define MAIN_H

#include <stdarg.h>
#include <unistd.h>

/*Prototype*/
int _printf(const char *format, ...);

/*prend un char, renvoie un int*/
int _putchar(char c);

/*int en retour, print l'argument écrit, nombre de caractères écrits. args=arguments */
int print_char(va_list args);
int print_string(va_list args);
int print_percent(va_list args);
int print_integer(va_list args);

int print_unsigned_number(unsigned int n);


#endif /* MAIN_H */