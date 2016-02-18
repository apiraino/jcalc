#include "common.h"
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
};
extern struct Patches patches[PATCH_last];

struct MEMDBG_reel
{
	LONG size;
	APTR addr;
    struct MEMDBG_reel *next;
};

struct Task *t;
extern STRPTR tskName; // replace this value in your code

/* function calls */

#ifndef ENABLE_MEMDBG
#define ENABLE_MEMDBG 0
#endif

#if ENABLE_MEMDBG == 1
    // public functions
    #define MEMDBG_start_tracing(x) _MEMDBG_start_tracing(x)
    #define MEMDBG_stop_tracing()    _MEMDBG_stop_tracing()
    #define MEMDBG_report_tracing()  _MEMDBG_report_tracing()

    // private functions
    exit_t _MEMDBG_start_tracing(BOOL);
    exit_t _MEMDBG_stop_tracing();
    exit_t _MEMDBG_report_tracing();
    
    exit_t MEMDBG_printList();
    exit_t MEMDBG_freeList();
#else
    #define MEMDBG_start_tracing(x) (void) EXIT_SUCCESS
    #define MEMDBG_stop_tracing() (void) EXIT_SUCCESS
    #define MEMDBG_report_tracing() (void) EXIT_SUCCESS
    
    exit_t MEMDBG_printList();
    exit_t MEMDBG_freeList();
#endif

#define BUFSIZE 100