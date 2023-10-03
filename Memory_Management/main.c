#include <stdio.h>
#include "my_allocation.h"
#include <unistd.h>
static void malloc_only_test(void) 
{
	int i, j;
	char * buf;
	char * oldbuf = NULL;
	char * bufs[100 - 2];
    
	for (i = 2; i < 100; i++) {
		buf = (char *)my_malloc(i);
		if (oldbuf) {
			printf("  diff: %lu\n", (unsigned long)buf - (unsigned long)oldbuf);
		}
		oldbuf = buf;
		bufs[i - 2] = buf;
		for (j = 0; j < i - 1; j++) {
			buf[j] = '0' + (i % 10); // put digit into string
		}
		buf[i - 1] = '\0';  // nul terminate
	}
    
	for (i = 0; i < 100 - 2; i++) {
		printf("%d -> %s\n", i, bufs[i]);
	}
}

static void simple_free_test(void) 
{
	/* do some mallocs, free it all, do the mallocs again (sbrk should not move) */
	int i, j, len;
	char * bufs[100];
	void * cur_brk;
    printf("First time allocate\r\n");
	for (i = 0; i < 100; i++) 
    {
		len = 100 + i;
		bufs[i] = my_malloc(len);
		for (j = 0; j < len - 1; j++) 
        {
			bufs[i][j] = '0' + (i % 10);
		}
		bufs[i][len - 1] = '\0';
	}
	for (i = 0; i < 100; i++) 
    {
		printf("%02d [0x%08lX]: %s\n", i, (unsigned long)bufs[i], (char *)bufs[i]);
	}
	cur_brk = sbrk(0);
	for (i = 0; i < 100; i++) {
		my_free(bufs[i]);
	}
    printf("2nds time allocate\r\n");
	for (i = 0; i < 100; i++) {
		len = 100 + i;
		bufs[i] = my_malloc(len);
		for (j = 0; j < len - 1; j++) {
			bufs[i][j] = '0' + (i % 10);
		}
		bufs[i][len - 1] = '\0';
	}
	printf("== sbrk(0): bfr 0x%08lX, after 0x%08lX ==\n",
			(unsigned long)cur_brk,
			(unsigned long)sbrk(0));
	for (i = 0; i < 100; i++) 
    {
		printf("%02d [0x%08lX]: %s\n", i, (unsigned long)bufs[i], (char *)bufs[i]);
	}

}
int main(int argc, char *argv[])
{
    simple_free_test();
    return 0;
}