



dnl This file is `m4/libTwmacros.m4' : m4 macros to autogenerate libTwmacros.h
dnl Tell the user about this.
/* This file was automatically generated with `m4' from `m4/libTwmacros.m4', do not edit! */

#if TW_CAN_UNALIGNED != 0
typedef struct s_reply *reply;
struct s_reply {
    uldat Len, Serial, Code, Data;
};
# define MyLen  MyReply->Len
# define MyCode MyReply->Code
# define MyData (&MyReply->Data)
# define DECL_MyReply reply MyReply;
# define INIT_MyReply 
#else
# define DECL_MyReply byte *MyReply, *MyData; uldat MyLen, MyCode;
# define INIT_MyReply (Pop(MyReply,uldat,MyLen), /*skip Serial:*/ Pop(MyReply,uldat,MyCode), Pop(MyReply,uldat,MyCode), MyData=MyReply, MyReply-=3*sizeof(uldat)),
#endif

define(`TRIM', `translit(`$1', ` ')')

define(`CHAIN', `TRIM($1)`'TRIM($2)')

define(`NAME', `Tw_`'CHAIN($1,$2)')

define(`NSHIFT', `ifelse($1, 0, `shift($@)', `NSHIFT(decr($1), shift(shift($@)))')')

define(`tv', v)
define(`t_', _)
define(`tx', x)
define(`tV', V)
define(`tW', W)

define(`iV', `$1')
define(`iW', `$1')

define(`Tv', `void ')
define(`T_', `$1 ')
define(`Tx', `t$1 ')
define(`TV', `TW_CONST $1 *')
define(`TW', `TW_CONST $1 *')

define(`TYPE', `T$2($1)')

define(`A', `a$1')
define(`AL', `len$1')

define(`ARG', `ifelse($3, v, `', `TYPE($2, $3)`'A($1)')')

define(`ARG0', `ifelse($3, v, `', `ARG($1, $2, $3);
    DECL_MyReply')')

define(`_ARGS', `ifelse($#, 2, `', `, ARG($1,$2,t$3)`'_ARGS(incr($1), NSHIFT(3, $@))')')
define(`ARGS', `tw_d TwD`'ifelse($#, 2, `', `_ARGS($@)')')

define(`lenARG', `ifelse($3, V, `
    uldat AL($1);', $3, W, `
    uldat AL($1);', `')')
define(`lenARGS', `ifelse($#, 2, `', `lenARG($1,$2,t$3)`'lenARGS(incr($1), NSHIFT(3, $@))')')

define(`SPACE_', `sizeof($2)')
define(`PARSE_', `Push(s,$2,A($1));')

define(`SPACEx', `sizeof(uldat)')
define(`PARSEx', `Push(s,uldat,A($1));')

define(`SPACEV', `(AL($1) = ($3) * sizeof($2))')
define(`PARSEV', `PushV(s,AL($1),A($1));')

define(`SPACEW', `(AL($1) = A($1) ? ($3) * sizeof($2) : 0, sizeof(uldat) + AL($1))')
define(`PARSEW', `Push(s,uldat,AL($1)); PushV(s,AL($1),A($1));')

define(`SPACE', `SPACE$3($1,$2,$4)')
define(`PARSE', `PARSE$3($1,$2,$4)')

define(`_SPACES', `ifelse($#, 2, `', `+ SPACE($1,$2,t$3,i$3) _SPACES(incr($1), NSHIFT(3, $@))')')
define(`SPACES', `ifelse($#, 2, `', `0 _SPACES($@)')')
define(`PARSES', `ifelse($#, 2, `', `PARSE($1,$2,t$3,i$3) PARSES(incr($1), NSHIFT(3, $@))')')

define(`GROW', `ifelse($1, `', `$2', `My = ($1);
            if (WQLeft(My)) {
                $2
            }')')

define(`RETv', `$4`'return;')
define(`RET_', `if ((MyReply = (void *)Wait4Reply(TwD, My)) && (INIT_MyReply MyCode == OK_MAGIC)) {
			if (MyLen == 2*sizeof(uldat) + sizeof(TYPE($2,$3)))
#if TW_CAN_UNALIGNED != 0
			    A(0) = *(TYPE($2,$3) *)MyData;
#else
			    Pop(MyData, TYPE($2,$3), A(0));
#endif
			else {
			    FailedCall(TwD, TW_ESTRANGE_CALL, order_$1);
			    A(0) = (TYPE($2,$3))TW_NOID;
			}
		    } else {
			FailedCall(TwD, MyReply && MyReply->Code != (uldat)-1 ?
				   TW_EFAILED_ARG_CALL : TW_EFAILED_CALL, order_$1);
			A(0) = (TYPE($2,$3))TW_NOID;
		    }
		    if (MyReply)
			KillReply(TwD, (byte *)MyReply, MyLen);
		    $4`'return A(0);')
define(`RETx', `RET_($@)')
define(`RET', `Send(TwD, (My = NextSerial(TwD)), id_Tw[order_$1]);
                    RET$3($1,$2,$3,UNLK;)')

define(`NOUNLK_RET', `Send(TwD, (My = NextSerial(TwD)), id_Tw[order_$1]);
                    RET$3($1,$2,$3,)')

define(`FAILv')
define(`FAIL_', `
    return ($1)0;')
define(`FAILx', `
    return TW_NOID;')
define(`FAIL',  `FAIL$2($1)')

define(`lockSELF', `
    uldat My;$3
    if (Fd != TW_NOFD && ((My = id_Tw[order_$1]) != TW_NOID ||
		       (My = FindFunctionId(TwD, order_$1)) != TW_NOID)) {
	if (InitRS(TwD)) {
            $2
	}
	/* still here? must be out of memory! */
	Errno = TW_ENO_MEM;
	Fail(TwD);
    } else if (Fd != TW_NOFD)
	FailedCall(TwD, TW_ENO_FUNCTION, order_$1);
    $4')

define(`NOSELF', `
    /* exception: Tw_FindFunction_() must be called WITH LOCK HELD! */
    uldat My;
    if (Fd != TW_NOFD) {
	id_Tw[order_$1] = FIND_MAGIC;
	if (InitRS(TwD)) {
	    $2
	}
	/* still here? must be out of memory! */
	Errno = TW_ENO_MEM;
	Fail(TwD);
    }')

define(`SELF', `lockSELF($@, `
    LOCK;', `
    UNLK;')')
define(`NOLOCK_SELF', `lockSELF($@,,)')

define(`PROTO', `TYPE($1, t$2)`'NAME($3, $4)(ARGS(1, NSHIFT(5, $@))) {
    ARG0(0, $1, t$2)`'lenARGS(1, NSHIFT(5, $@))`'SELF(CHAIN($3, $4),
        GROW(SPACES(1, NSHIFT(5, $@)),
                    PARSES(1, NSHIFT(5, $@))
	            RET(CHAIN($3, $4), $1, $2)))`'FAIL($1, $2)
}')

define(`PROTOSyncSocket', `static TYPE($1, t$2)_`'NAME($3, $4)(ARGS(1, NSHIFT(5, $@))) {
    ARG0(0, $1, t$2)`'lenARGS(1, NSHIFT(5, $@))`'NOLOCK_SELF(CHAIN($3, $4),
        GROW(SPACES(1, NSHIFT(5, $@)),
                    PARSES(1, NSHIFT(5, $@))
	            NOUNLK_RET(CHAIN($3, $4), $1, $2)))`'FAIL($1, $2)
}')

define(`PROTOFindFunction', `static TYPE($1, t$2)_`'NAME($3, $4)(ARGS(1, NSHIFT(5, $@))) {
    ARG0(0, $1, t$2)`'lenARGS(1, NSHIFT(5, $@))`'NOSELF(CHAIN($3, $4),
        GROW(SPACES(1, NSHIFT(5, $@)),
                    PARSES(1, NSHIFT(5, $@))
	            NOUNLK_RET(CHAIN($3, $4), $1, $2)))`'FAIL($1, $2)
}')

include(`m4/sockproto.m4')

