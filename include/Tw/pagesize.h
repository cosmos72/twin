#ifndef _TW_PAGESIZE_H
#define _TW_PAGESIZE_H

#ifndef _TW_TWAUTOCONF_H
# include <Tw/autoconf.h>
#endif

#ifdef TW_HAVE_SYS_MMAN_H
# include <sys/mman.h>
#endif

/* This mess was adapted from the GNU getpagesize.h.  */
#ifdef TW_HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifndef TW_HAVE_GETPAGESIZE
# if defined(_SC_PAGESIZE) && defined(TW_HAVE_SYSCONF)
#  define getpagesize() (sysconf(_SC_PAGESIZE))
# else /* no sysconf(_SC_PAGESIZE) */
#  ifdef TW_HAVE_ASM_PAGE_H
#   include <asm/page.h>
#  endif
#  ifdef TW_HAVE_SYS_PARAM_H
#   include <sys/param.h>
#  endif
#  ifdef EXEC_PAGESIZE
#   define getpagesize() (EXEC_PAGESIZE)
#  else /* no EXEC_PAGESIZE */
#   ifdef NBPG
#    ifndef CLSIZE
#     define CLSIZE 1
#    endif /* no CLSIZE */
#    define getpagesize() ((NBPG) * (CLSIZE))
#   else /* no NBPG */
#    ifdef NBPC
#     define getpagesize() (NBPC)
#    else /* no NBPC */
#     ifdef PAGE_SIZE
#      define getpagesize() (PAGE_SIZE)
#     else /* no PAGE_SIZE */
#      ifdef PAGESIZE      
#	define getpagesize() (PAGESIZE)
#      else
#	error cannot detect PAGESIZE
#      endif /* no PAGESIZE */
#     endif /* no PAGE_SIZE */
#    endif /* no NBPC */
#   endif /* no NBPG */
#  endif /* no EXEC_PAGESIZE */
# endif /* no _SC_PAGESIZE */
#endif /* no TW_HAVE_GETPAGESIZE */

#endif /* _TW_PAGESIZE_H */
