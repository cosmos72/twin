
#ifndef _TUTF_COMPILER_H
#define _TUTF_COMPILER_H

/* library stuff. define macros with TUTF_ prefix */


#define TUTF_CAT(a,b)		TW__CAT(a,b)
#define TUTF_CAT3(a,b,c)	TW__CAT3(a,b,c)
#define TUTF_CAT4(a,b,c,d)	TW__CAT4(a,b,c,d)
#define T_CAT(a,b)		TW__CAT(a,b)
#define T_CAT3(a,b,c)		TW__CAT3(a,b,c)
#define T_CAT4(a,b,c,d)		TW__CAT4(a,b,c,d)

#define TUTF_STR(s)		TW__STR(s)

#define T_STR(s)		TW__STR(s)

#define TUTF_CONST		TW_CONST
#define TUTF_INLINE		TW_INLINE
#define TUTF_VOLATILE		TW_VOLATILE
#define TUTF_FN_ATTR_CONST	TW_FN_ATTR_CONST
#define TUTF_TYPE_ATTR_PACKED	TW_TYPE_ATTR_PACKED

#endif /* _TUTF_COMPILER_H */

