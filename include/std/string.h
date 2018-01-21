#ifndef string_h
#define string_h

#include <std/int.h>

int strncmp(char *, char *, size_t);
char *strfnd(char *, char);
uint64_t hex2int(char *);
uint64_t strlen(char *);
void *memcpy(void *, void *, size_t);
void *memset(void *, int b, size_t);
char hexr(char);

#endif