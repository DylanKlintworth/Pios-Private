#include <stddef.h>
void * memcpy(void * __restrict dest, const void * __restrict src, size_t num);
void *memset(void *s, int c, size_t n);
int strcmp ( const char * str1, const char * str2 );
int strncmp ( const char * str1, const char * str2, size_t n );
int toupper(int c);
char * strcpy ( char * destination, const char * source );
char * strncpy ( char * destination, const char * source, unsigned int length);
char* strcat(char* destination, const char* source);
