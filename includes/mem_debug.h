#include <proto/arossupport.h> /* for RawPutChars */

enum {LIB_Exec, LIB_last};

#define LVO_AllocVec	( -114 * (WORD)LIB_VECTSIZE)
#define LVO_FreeVec		( -115 * (WORD)LIB_VECTSIZE)

struct Library *libbases[LIB_last];
typedef LONG (*FP)();

enum {
	PATCH_AllocVec,
	PATCH_FreeVec,
	PATCH_last
};

struct Patches
{
    LONG (*oldfunc)();
    LONG (*newfunc)();
    LONG libidx;
    struct Library *lib;
    LONG lvo;
} patches[PATCH_last] = 
{
    {NULL, NULL, LIB_Exec, 0, LVO_AllocVec},
    {NULL, NULL, LIB_Exec, 0, LVO_FreeVec},
};

struct MEMDBG_reel
{
	LONG size;
	APTR addr;
    struct MEMDBG_reel *next;
};

struct Task *t;
STRPTR tskName = (STRPTR)"MyTaskName"; // replace this value in your code
static struct MEMDBG_reel *list = NULL;
static ULONG count = 0; /* allocations count for reporting purposes */
static ULONG total = 0; /* total allocations byte count for reporting purposes */
static BOOL saveOnDisk = FALSE;

/* "public" function calls */

#ifndef ENABLE_MEMDBG
#define ENABLE_MEMDBG 0
#endif

exit_t MEMDBG_start_tracing(BOOL);
exit_t MEMDBG_stop_tracing();
exit_t MEMDBG_report_tracing();
exit_t MEMDBG_printList();
exit_t MEMDBG_freeList();

#define BUFSIZE 100