#ifndef _MACH_I386_THREAD_STATUS_H_
#define _MACH_I386_THREAD_STATUS_H_

#include <architecture/i386/frame.h>    /* FIXME */
#include <architecture/i386/fpu.h>      /* FIXME */

/*     THREAD_STATE_FLAVOR_LIST 0 */
#define i386_NEW_THREAD_STATE   1       /* used to be i386_THREAD_STATE */
#define i386_FLOAT_STATE        2
#define i386_ISA_PORT_MAP_STATE 3
#define i386_V86_ASSIST_STATE   4
#define i386_REGS_SEGS_STATE    5
#define THREAD_SYSCALL_STATE    6
//#define THREAD_STATE_NONE       7
#define i386_SAVED_STATE        8

#define i386_THREAD_STATE       -1

/*typedef struct {
  unsigned int        eax;
  unsigned int        ebx;
  unsigned int        ecx;
  unsigned int        edx;
  unsigned int        edi;
  unsigned int        esi;
  unsigned int        ebp;
  unsigned int        esp;
  unsigned int        ss;
  unsigned int        eflags;
  unsigned int        eip;
  unsigned int        cs;
  unsigned int        ds;
  unsigned int        es;
  unsigned int        fs;
  unsigned int        gs;
} i386_thread_state_t;
*/
#define _STRUCT_X86_THREAD_STATE32  struct __darwin_i386_thread_state
_STRUCT_X86_THREAD_STATE32
{
    unsigned int    __eax;
    unsigned int    __ebx;
    unsigned int    __ecx;
    unsigned int    __edx;
    unsigned int    __edi;
    unsigned int    __esi;
    unsigned int    __ebp;
    unsigned int    __esp;
    unsigned int    __ss;
    unsigned int    __eflags;
    unsigned int    __eip;
    unsigned int    __cs;
    unsigned int    __ds;
    unsigned int    __es;
    unsigned int    __fs;
    unsigned int    __gs;
};
typedef _STRUCT_X86_THREAD_STATE32 i386_thread_state_t;

#define _STRUCT_X86_THREAD_STATE64  struct __darwin_x86_thread_state64
_STRUCT_X86_THREAD_STATE64
{
    uint64_t  __rax;
    uint64_t  __rbx;
    uint64_t  __rcx;
    uint64_t  __rdx;
    uint64_t  __rdi;
    uint64_t  __rsi;
    uint64_t  __rbp;
    uint64_t  __rsp;
    uint64_t  __r8;
    uint64_t  __r9;
    uint64_t  __r10;
    uint64_t  __r11;
    uint64_t  __r12;
    uint64_t  __r13;
    uint64_t  __r14;
    uint64_t  __r15;
    uint64_t  __rip;
    uint64_t  __rflags;
    uint64_t  __cs;
    uint64_t  __fs;
    uint64_t  __gs;
};
typedef _STRUCT_X86_THREAD_STATE64 x86_thread_state64_t;


#define i386_THREAD_STATE_COUNT         \
    ( sizeof (i386_thread_state_t) / sizeof (int) )

#define i386_THREAD_FPSTATE     -2

typedef struct {
  fp_env_t            environ;
  fp_stack_t          stack;
} i386_thread_fpstate_t;

#define i386_THREAD_FPSTATE_COUNT       \
    ( sizeof (i386_thread_fpstate_t) / sizeof (int) )

#define i386_THREAD_EXCEPTSTATE -3

typedef struct {
  unsigned int        trapno;
  err_code_t          err;
} i386_thread_exceptstate_t;

#define i386_THREAD_EXCEPTSTATE_COUNT   \
    ( sizeof (i386_thread_exceptstate_t) / sizeof (int) )

#define i386_THREAD_CTHREADSTATE        -4

typedef struct {
  unsigned int        self;
} i386_thread_cthreadstate_t;

#define i386_THREAD_CTHREADSTATE_COUNT  \
    ( sizeof (i386_thread_cthreadstate_t) / sizeof (int) )

#define USER_CODE_SELECTOR      0x0017
#define USER_DATA_SELECTOR      0x001f

#endif
