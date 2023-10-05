/******************************************************************************
 *                              HEADER AND DIRECTORY 
******************************************************************************/
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


/******************************************************************************
 *                              DEFINE AND TYPEDEF 
******************************************************************************/
typedef struct block_free_header 
{
    size_t block_length;
    struct block_free_header * prev_free_chunk;
    struct block_free_header * next_free_chunk;
} block_free_header_t;

typedef struct block_allocated_header
{
    size_t block_length;
} block_allocated_header_t;

#if(DEBUG_MY_ALLOCATION)
#define DEBUG_MY_ALLOCATION(format_, ...)  printf(format_, ##__VA_ARGS__)
#else
#define DEBUG_MY_ALLOCATION(format_, ...) (void)(0)
#endif

#define FREE_BLOCK_HEADER(ptr) ((block_free_header_t *) ((unsigned long)ptr - sizeof(block_free_header_t)))
#define FREE_BLOCK_MEMORY(ptr) ((void *)((unsigned long)ptr + sizeof(block_free_header_t)))


/******************************************************************************
 *                              PRIVATE VARIABLES
******************************************************************************/
static block_free_header_t * m_free_ll_head = NULL;
/*
 * Remove the specified list_item from the free nodes linked list
 */
static int ll_remove(block_free_header_t * list_item)
{
	/* make prev and next point to each other, if not NULL */
	if (!list_item->prev_free_chunk) 
    { // The previous free is NULL
		if (list_item->next_free_chunk) 
        {
			m_free_ll_head = list_item->next_free_chunk;
		} 
        else 
        {
			m_free_ll_head = NULL;
		}
	} 
    else 
    {    /*The prevous point to some me */
		list_item->prev_free_chunk->next_free_chunk = list_item->next_free_chunk;
	}
	if (list_item->next_free_chunk) 
    {
		list_item->next_free_chunk->prev_free_chunk = list_item->prev_free_chunk;
	}
	return 0;
}

/*
 * @brief Replace the first list_item with the second list_item in the
 * freed nodes linked list.
 */
static int ll_replace(block_free_header_t *old_item, block_free_header_t *new_item)
{
	/* update references that must point to new item */
	if (old_item->prev_free_chunk)
		old_item->prev_free_chunk->next_free_chunk = new_item;
	else
		m_free_ll_head = new_item;

	if (old_item->next_free_chunk)
		old_item->next_free_chunk->prev_free_chunk = new_item;

	/* update references for the new item itself */
	new_item->prev_free_chunk = old_item->prev_free_chunk;
	new_item->next_free_chunk = old_item->next_free_chunk;
	return 0;
}


/* The malloc() function allocates size bytes and returns a pointer to
 * the allocated memory. The memory is not initialized. If size is 0,
 * then malloc() returns either NULL, or a unique pointer value that can
 * later be successfully passed to free().
 * ----
 * Implementation notes:
 * So, somebody wants a block of memory... fine!  Here's the game
 * plan for getting them said memory:
 * 
 * 1) Scan each block in our free list to see if any of them are
 *    big enough.  If we find one that is big enough, we will
 *    either use that memory as-is or chunk it into a couple smaller
 *    pieces (part of which is used, the other which is a new
 *    entry in the free linked list.
 * 2) If we do not find an item in the free list, we will add the
 *    new block onto the end of our heap.  We will attempt to expand
 *    the heap if needed.
 */
void *my_malloc(size_t size)
{
    block_free_header_t *current_header, *new_free_header, *block_header;
    void *memory_to_use = NULL;
    const size_t size_plus_header = size + sizeof(block_free_header_t);
    new_free_header = block_header = NULL;
    current_header = m_free_ll_head;
    while (current_header) 
    {
        if (current_header->block_length >= size) 
        {
            memory_to_use = FREE_BLOCK_MEMORY(current_header);
            if (current_header->block_length > size_plus_header) 
            {  
                /* The require block is smaller than the current free block case*/
                new_free_header = (block_free_header_t *)((unsigned long)memory_to_use + size);  /*Insert a new meta data block*/
                new_free_header->block_length = current_header->block_length - size; /*Spliiting block*/
                ll_replace(current_header, new_free_header); /*Current metadata block is not point to the free area -> Replace it with new one*/
                
            } 
            else 
            {   /*equal case*/
            	ll_remove(current_header);
            }
            /*Case when not have enough size to store the header*/
            return memory_to_use;
        } 
        else 
        {
            current_header = current_header->next_free_chunk;
        }
    }

    size_t expand_size;
    if (current_header) {
    	expand_size = (size_plus_header - current_header->block_length);
    } else {
    	expand_size = size_plus_header;
    }
    block_header = (block_free_header_t *)sbrk(expand_size);
    memory_to_use = FREE_BLOCK_MEMORY(block_header);
    if (!block_header) {
    	/* failed to allocate more memory, return NULL */
    	return NULL;
    }

    /* current_header points to end of current LL unless first */
	block_header->next_free_chunk = NULL;
	block_header->prev_free_chunk = NULL;
	block_header->block_length = size;
    return memory_to_use;
}
/* The malloc() function allocates size bytes and returns a pointer to
 * the allocated memory. The memory is not initialized. If size is 0,
 * then malloc() returns either NULL, or a unique pointer value that can
 * later be successfully passed to free().
 * ----
 * Implementation notes:
 * So, somebody wants a block of memory... fine!  Here's the game
 * plan for getting them said memory:
 * 
 * 1) Check if the length of ptr point to block is greater, equal or less than 
 *    size request.
 *    + Greater: Allocate new heap region and copy the old data, free the old region
 *    + Equal: Returen the data
 *    + Less: Resize the length and mark the unused on to be free.
 *  //**TODO: Some time we rellocate the size that the unused data doesn't have enough space
 *    to store the meta data -> We need strictly implementation. 
 *   Because of the simple of the assignment -> I will make it as TODO :v
 */
void *my_realloc(void* ptr, size_t size)
{
    const size_t size_plus_header = size + sizeof(block_free_header_t);
    void *memory_to_use = NULL;
    block_free_header_t *current_header, *block_header, *new_free_header;
    current_header = FREE_BLOCK_HEADER(ptr);
    if(ptr == NULL || size == 0)
    {
        /**/
        return NULL;
    }
    if(size > current_header->block_length)
    {
        /*Perform allocate new memory*/
        size_t expand_size = size_plus_header;
        block_header = (block_free_header_t*)sbrk(expand_size);
        /*Copy from old allocate area to new allocated area*/
        memcpy(FREE_BLOCK_MEMORY(block_header), ptr, current_header->block_length); 
        memory_to_use = FREE_BLOCK_MEMORY(block_header);
        /*Make old area as free space*/
        free(ptr);
        return memory_to_use;
    }
    else if (size < current_header->block_length)
    {
        current_header = FREE_BLOCK_HEADER(ptr);
        current_header->block_length = size;
        memory_to_use = ptr;
        new_free_header = (block_free_header_t *)((unsigned long)memory_to_use + size);  /*Insert a new meta data block*/
        new_free_header->block_length = current_header->block_length - size; /*Spliiting block*/
        ll_replace(current_header, new_free_header); /*Current metadata block is not point to the free area -> Replace it with new one*/
    }
    else
    {
        return ptr;
    }
    /*Implement the new size*/
}
/*
 * The free() function frees the memory space pointed to by ptr, which
 * must have been returned by a previous call to malloc(), calloc() or
 * realloc(). Otherwise, or if free(ptr) has already been called before,
 * undefined behavior occurs. If ptr is NULL, no operation is performed.
 * ----
 * Implementation notes:
 * On free, we are given a pointer to an area of memory that is no longer
 * needed.  Immediately preceding this memory we should find a
 * memory_block_free_header_t with the length of the block.  Our full list of
 * steps are as follows:
 * 
 * 1) Add forward reference from previous free block to this free block.
 * 2) Add backref from this free block to previous free block.
 * 3) Other small stuff (set next pointer to NULL, etc.)
 * 
 * Basically, we are maintaining a linked list of freed blocks that we
 * may do with as we please.
 * 
 * TODO: we currently don't set the break to ungrow the heap space for this
 * process.  This should be added in the future.
 *
 * TODO: we currently don't scan the entire linked list or manage blocks in
 * such a way that we can avoid fragmentation.  For instance, we leave a bunch
 * of gaps on the table when running simple_free_test().
 */
void my_free(void *ptr)
{
    /*Just do nothing*/
    if (ptr == NULL)
    {
        return;
    }
	/* we just insert this item into the front of the linked list.  Note
	 * that we don't touch the length as we assume this is still correct.*/
	block_free_header_t * new_freed_header = FREE_BLOCK_HEADER(ptr);
	new_freed_header->prev_free_chunk = NULL;
	if (m_free_ll_head) 
    {
		new_freed_header->next_free_chunk = m_free_ll_head;
		m_free_ll_head->prev_free_chunk = ptr; //? PTR
		m_free_ll_head = new_freed_header;
	} 
    else 
    {
		new_freed_header->next_free_chunk = NULL;
		m_free_ll_head = new_freed_header;
	}
#if(0) /*Since I don't see The Linux Programming Interface says anything about merging  -> This is optional*/
    /*We don't know the continous blocks so we have to scan all the current block when free*/
    /*Doing merging block*/
    block_free_header_t *current_header = m_free_ll_head;
    block_free_header_t *temp_header = NULL;
    while(current_header)
    {
        if(current_header->next_free_chunk != NULL)
        {
            temp_header = (block_free_header_t *)(current_header + current_header->block_length + sizeof(current_header)); /*Find continous free block*/
            if(temp_header == current_header->next_free_chunk)  
            {
                /*Perform merging*/
                
                
            }
        }

    }
#endif
}

