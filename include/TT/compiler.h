
#ifndef _TT_COMPILER_H
#define _TT_COMPILER_H

/** macro to concatenate args */
#define TT_CAT(a,b)		TW__CAT(a,b)
#define TT_CAT3(a,b,c)		TW__CAT3(a,b,c)
#define TT_CAT4(a,b,c,d)	TW__CAT4(a,b,c,d)
/** macro to stringify an arg */
#define TT_STR(a)		TW__STR(a)
/** `const' if your compiler understands it, else nothing */
#define TT_CONST		TW_CONST
/** `inline' or an appropriate variation if your compiler understands it, else 'static' */
#define TT_INLINE		TW_INLINE
/** define if your compiler can inline */
#define TT_CAN_INLINE		TW_CAN_INLINE
/** `volatile' if your compiler understands it, else nothing */
#define TT_VOLATILE		TW_VOLATILE
/** `__attribute__((const))' if your compiler understands it, else nothing */
#define TT_FN_ATTR_CONST	TW_FN_ATTR_CONST
/** `__attribute__((packed))' if your compiler understands it, else nothing */
#define TT_TYPE_ATTR_PACKED	TW_TYPE_ATTR_PACKED


/*
 * function arguments attributes:
 * if no attribute is specified, TT_ARG_WRITE is assumed as default.
 */

/**
 * object can only be read (NOT written) by the function until it returns;
 * function does not own the object and cannot store object value
 * for later use after function returned.
 */
#define TT_ARG_READ		TT_CONST
/**
 * object can only be written (NOT read or destroyed) by the function until it returns;
 * function does not own the object, but can store object value for later use as long as
 * it sets up a mechanism to trash the object value upon or before object destruction.
 */
#define TT_ARG_WRITE

/**
 * object can only be read/written (NOT destroyed) by the function until it returns;
 * function does not own the object, but can store object value for later use as long as
 * it sets up a mechanism to trash the object value upon or before object destruction.
 */
#define TT_ARG_READ_WRITE

/**
 * object is donated to the function, which owns the object and can do anything with it.
 * caller cannot store object value for later use after function is invoked.
 */
#define TT_ARG_DIE

/**
 * expression giving array length
 */
#define TT_ARG_ARRAY(expr)
/**
 * array is zero-terminated
 */
#define TT_ARG_ARRAY_Z

#endif /* _TT_COMPILER_H */

