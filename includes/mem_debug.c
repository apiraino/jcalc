/*
	Basic tool to patch memory allocation/deallocation functions.
	Purpose is to track memory usage of your own program and find
	leaks.
	Why not the AROS resource tracker?
	http://aros.sourceforge.net/documentation/developers/app-dev/resource-tracking.php
	Because it does not track a missing Freevec (which is what I need)

	0.1
	Patch AllocVec/FreeVec. Just list them, no checks are performed
	Code is almost copy&pasted from Scout.

	0.2
	Added logging facility

*/

#include "mem_debug.h"

#include <aros/debug.h>

exit_t MEMDBG_freeList(struct MEMDBG_reel *l)
{
	// TODO
	free(l); l = NULL;
	/*
	struct mydata *item = NULL;
	if (NULL != l)
	{
		item = l;
		while(NULL != item)
		{
			item = l->next;
			free(l); l = NULL;
		}
	}
	free(item; item = NULL;
	*/

	return EXIT_SUCCESS;
}

exit_t MEMDBG_printList(struct MEMDBG_reel **mylist)
{
	struct MEMDBG_reel *l = *mylist;
	APTR *fp;

	char buf[BUFSIZE]; buf[0] = '\0';
	
	if (saveOnDisk == TRUE)
	{
		STRPTR logfile = (STRPTR)"JMAN:MEMDBG.log";
		/* See dos/dos.h for file opening modes */
		if (NULL == (fp = Open(logfile, MODE_NEWFILE)))
		{
			printf("Could not open file %s\n", logfile);
		}
	}

	while (NULL != l)
	{
		// FIXME: How do I remove the head when instancing the list?
		if (strcmp((char *)l->addr, "HEAD") == 0)
		{
			l = l->next;
			continue;
		}

		total += l->size;

		buf[0] = '\0';
		sprintf(buf, ">[%d] At 0x%p: size=%d addr=0x%p -> 0x%p\n",
				count, l, l->size, l->addr, l->next);

		if (saveOnDisk == TRUE)
		{
			Write(fp, buf, strlen(buf));
		}
		else
		{
			RawPutChars((STRPTR)buf, strlen(buf));
		}

		l = l->next;
		count++;
	}

	if (count != 0)
	{
		D(bug("Why?"));
		asm volatile("int $3");
	}


	buf[0] = '\0';
	sprintf(buf, "Reporting memory usage: %d allocation(s), %d bytes\n", count, total);
 
	if (saveOnDisk == TRUE)
	{
		Write(fp, buf, strlen(buf));
		Close(fp);
	}
	else
	{
		RawPutChars((STRPTR)buf, strlen(buf));
	}

	return EXIT_SUCCESS;
}

exit_t MEMDBG_remNodeFromList(struct MEMDBG_reel *list, STRPTR addr)
{
	// D(bug(">>>> Remove addr=0x%p\n", addr));
	// D(bug("[remNodeFromList] Removing node 0x%p\n", addr));
	// printf("[remNodeToList] New position: 0x%p\n", l);

	if (addr == NULL)
	{
		D(bug(">>> ERROR requested to remove NULL node\n"));
		asm volatile("int $3");
		return EXIT_FAILURE;
	}

	struct MEMDBG_reel *l = list;
	struct MEMDBG_reel *prev;

	/* Parse che list until you reach the end or find the Node I request */
	while(l != NULL)
	{
		// D(bug("0x%p: checking 0x%p against 0x%p\n", l, l->addr, addr));

		// If I have the address I want
		// if (strncmp((char *)l->addr, (char *)addr, strlen((char *)addr)) == 0)
		if (l->addr == addr)
		{
			// D(bug(">>> addr=%p is equal to addr=%p\n", l->addr, addr));
			break;
		}
		else
		{
			// go on even if we are the tail, it will set to NULL the current item
			// and at the next iteration, the iteration will break.
			prev = l;
			// if (l->next != NULL)
			{
				l = l->next;
			}
		}		
	}

	// D(bug("[remNodeFromList] Will unlink node with addr=0x%p (0x%p), node's next will point to 0x%p\n",
	// 	addr, prev, l));

	// Found the node to be removed.
	if (l != NULL)
	{
		// printf("> Item to be removed is at 0x%p (%s)\n", l, l->addr);
		// printf("> His previous is at 0x%p (%s)\n", prev, prev->addr);

		// if next is NULL I have the tail of the list
		if (l->next != NULL)
		{
			// not the tail, assign item's next ptr to previous one
			prev->next = l->next;
		}
		else
		{
			// Ensure the tail points to NULL
			// D(bug(">>>> Tail!!!\n"));
			prev->next = NULL;
		}

		// and eliminare the item
		free(l); l = NULL;
	}

	// D(bug("[remNodeFromList] l is now 0x%p\n", l));

	return EXIT_SUCCESS;
}

exit_t MEMDBG_addNodeToList(struct MEMDBG_reel *l, ULONG byteSize, APTR addr)
{
	// D(bug("[addNodeToList] History reel starts @0x%p (-> 0x%p)\n", l, l->next));

	while (NULL != l->next)
	{
		l = l->next;
	}

	struct MEMDBG_reel *nuovo = malloc(sizeof(struct MEMDBG_reel));

	if (!nuovo)
	{
		char buf[BUFSIZE]; buf[0] = '\0';
		sprintf(buf, "[addNodeToList] Malloc failed for list, bail out!\n");
		RawPutChars((STRPTR)buf, strlen(buf));
		return EXIT_FAILURE;
	}

	// D(bug("[addNodeToList] Adding node: size=%d addr=0x%p (%s)\n", byteSize, addr, addr));

	// Cosi' ci mette il puntatore
	// Io vorrei la rappresentazione in stringa del suo indirizzo
	// nuovo->addr = (STRPTR)addr;

	nuovo->addr = addr;
	nuovo->size = byteSize;
	l->next = nuovo;
	nuovo->next	= NULL;

	return EXIT_SUCCESS;
}

exit_t MEMDBG_init()
{
	char buf[BUFSIZE]; buf[0] = '\0';
	if (NULL == (list = malloc(sizeof(struct MEMDBG_reel))))
	{
		sprintf(buf, "[MEM_DEBUG] ERROR Malloc failed for list, bail out!\n");
		RawPutChars((STRPTR)buf, strlen(buf));
		return EXIT_FAILURE;
	}
	list->size	= 0;
	list->addr	= (STRPTR)"HEAD";
	list->next	= NULL;

	buf[0] = '\0';
	sprintf(buf, "[MEM_DEBUG] INIT List allocated at 0x%p (-> 0x%p)\n", list, list->next);
	RawPutChars((STRPTR)buf, strlen(buf));

	return EXIT_SUCCESS;
}

AROS_LH2(APTR, New_AllocVec,
	AROS_LHA(ULONG, byteSize,	D0),
	AROS_LHA(ULONG, attributes,	D1),
    struct ExecBase *, SysBase, 114, Exec)
{
    AROS_LIBFUNC_INIT

    APTR result = AROS_CALL2(APTR, patches[PATCH_AllocVec].oldfunc,
		AROS_LDA(ULONG, byteSize,	D0),
		AROS_LDA(ULONG, attributes,	D1),
		struct ExecBase *, SysBase);

	struct Task *tsk = FindTask(NULL);
	if ( strcmp((char *)tskName, tsk->tc_Node.ln_Name) == 0 )
	{
		char text[BUFSIZE]; text[0] = '\0';
		sprintf(text, "[MEM_DEBUG] AllocVec ");

		char allocsize[BUFSIZE]; allocsize[0] = '\0';
		sprintf(allocsize, "bytes %d ", (int)byteSize);

		char allocaddr[BUFSIZE]; allocaddr[0] = '\0';
		sprintf(allocaddr, "at 0x%p\n", result);

		RawPutChars((STRPTR)text, strlen(text));
		RawPutChars((STRPTR)allocsize, strlen(allocsize));
		RawPutChars((STRPTR)allocaddr, strlen(allocaddr));

		MEMDBG_addNodeToList(list, byteSize, result);
	}
    
    return result;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(VOID, New_FreeVec,
    AROS_LHA(APTR, memoryBlock, A1),
    struct ExecBase *, SysBase, 115, Exec)
{
    AROS_LIBFUNC_INIT

    AROS_CALL1(VOID, patches[PATCH_FreeVec].oldfunc,
		AROS_LDA(APTR, memoryBlock,    A1),
		struct ExecBase *, SysBase);

	struct Task *tsk = FindTask(NULL);
	if ( strcmp((char *)tskName, tsk->tc_Node.ln_Name) == 0 )
    {
		char buf[BUFSIZE];
		sprintf(buf, "[MEM_DEBUG] FreeVec at 0x%p\n", memoryBlock);
	    RawPutChars((STRPTR)buf, strlen(buf));

		// char _memoryBlock[BUFSIZE]; _memoryBlock[0] = '\0';
		// sprintf(_memoryBlock, "0x%p", memoryBlock);

		// D(bug("[MEM_DEBUG] FreeVec removing node: addr=0x%p\n", memoryBlock));

    	if (EXIT_FAILURE == MEMDBG_remNodeFromList(list, memoryBlock))
    	{
    		D(bug("[MEM_DEBUG] ERROR memoryBlock is NULL!!! 0x%p (task=%s)\n", memoryBlock, tsk->tc_Node.ln_Name));
    		// MEMDBG_printList(&list);
    		// exit(1);
    	}
	}
    
    AROS_LIBFUNC_EXIT
}

exit_t MEMDBG_start_tracing(BOOL log)
{
	/* Setting up logging */
	saveOnDisk = log;
	char buf[BUFSIZE]; buf[0] = '\0';

	if (t == NULL)
	{
		printf("[MEM_DEBUG] tc_Node.ln_Name is NULL!!! Please configure it in your application\nBail out.\n");
	    exit(1);
	}

	MEMDBG_init();

	buf[0] = '\0';
	sprintf(buf, "[MEM_DEBUG] Start tracing for Task 0x%p (name='%s')\n", t, t->tc_Node.ln_Name);
    RawPutChars((STRPTR)buf, strlen(buf));

	// init
	libbases[LIB_Exec] = (struct Library*)SysBase;
	patches[PATCH_AllocVec].newfunc	= (FP)AROS_SLIB_ENTRY(New_AllocVec, Exec);
	patches[PATCH_FreeVec].newfunc	= (FP)AROS_SLIB_ENTRY(New_FreeVec, Exec);

	// replace functions
	int i;
	for (i = 0; i < PATCH_last; i++)
	{
	    Forbid();
	    patches[i].oldfunc = SetFunction(libbases[patches[i].libidx], patches[i].lvo, patches[i].newfunc);
	    Permit();
	}

	return EXIT_SUCCESS;
}

exit_t MEMDBG_stop_tracing()
{
 	// restore original functions
 	int i;
	for (i = 0; i < PATCH_last; i++)
	{
	    Forbid();
		SetFunction(libbases[patches[i].libidx], patches[i].lvo, patches[i].oldfunc);
		Permit();
		patches[i].oldfunc = NULL;
	}

	char buf[BUFSIZE]; buf[0] = '\0';
	sprintf(buf, "[MEM_DEBUG] Stop tracing for Task 0x%p\n", t);
    RawPutChars((STRPTR)buf, strlen(buf));

    // MEMDBG_freeList();

    return EXIT_SUCCESS;
}

exit_t MEMDBG_report_tracing()
{
	// report
	MEMDBG_printList(&list);

	char buf[BUFSIZE]; buf[0] = '\0';
	sprintf(buf, "[MEM_DEBUG] Reporting memory usage: %d allocation(s), %d bytes\n", count, total);
    RawPutChars((STRPTR)buf, strlen(buf));

    return EXIT_SUCCESS;
}