
#ifndef _TT_COMPILER_H
#define _TT_COMPILER_H

/** macro to concatenate two args */
#define TT_CAT(a,b)		TW__CAT(a,b)
/** macro to stringify an arg */
#define TT_STR(a)		TW__STR(a)
/** `const' if you compiler understands it, else nothing */
#define TT_CONST		TW_CONST
/** `inline' or an appropriate variation if you compiler understands it, else nothing */
#define TT_INLINE		TW_INLINE
/** `volatile' if you compiler understands it, else nothing */
#define TT_VOLATILE		TW_VOLATILE
/** `__attribute__((const))' if you compiler understands it, else nothing */
#define TT_FN_ATTR_CONST	TW_FN_ATTR_CONST
/** `__attribute__((packed))' if you compiler understands it, else nothing */
#define TT_TYPE_ATTR_PACKED	TW_TYPE_ATTR_PACKED

#endif /* _TT_COMPILER_H */

