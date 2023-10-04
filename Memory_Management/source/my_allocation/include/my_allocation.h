#ifndef __MY_ALLOCATION_H__
#define __MY_ALLOCATION_H__

#include <stdlib.h>
/**
 * @brief Allocated dynamic memory
 * 
 * @param[in] size data size we want to allocate
 * @return Pointer point to data address can be used
 */
void *my_malloc(size_t size);
/**
 * @brief Resize dynamic memory
 * 
 * @param[in] size data size we want to allocate
 * @param[in] ptr Pointer return by other API like  my_malloc
 * @return Pointer point to data address can be used
 */
void *my_realloc(void* ptr, size_t size);
/**
 * @brief Free the allocated memory
 * 
 * @param[in] ptr Pointer points to allocated memory return by my_malloc
 */
void my_free(void *ptr);
#endif