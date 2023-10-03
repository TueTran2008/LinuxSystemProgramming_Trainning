#ifndef __MY_ALLOCATION_H__
#define __MY_ALLOCATION_H__

#include <stdlib.h>
void *my_malloc(size_t size);

void *my_realloc(void* ptr, size_t size);

void my_free(void *ptr);
#endif