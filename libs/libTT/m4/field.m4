

dnl This file is `m4/field.m4' : m4 macros to autogenerate field_m4.h
dnl Tell the user about this.


/* This file was automatically generated from `m4/field.m4', do not edit! */


/*
 *  field_m4.h  --  arrays for field.c conversion functions
 *                  between strings and numeric ids
 */

include(`m4/TTclasses.m4h')
include(`m4/TThandy.m4h')

divert(-1)

define(`body_fn_get', `
    { this`'_$4, len(this`'_$4), "this`'_$4", },')

define(`def_handy', `ifelse(`$3', get, `body_fn_$3($@)')')

define(`wrap_export', `ifelse(index(`$3', `r'), -1, `', `exported($1,,get,$2,$1)')`'dnl
`'ifelse(index(`$3', `w'), -1, `', `exported(void,,set,$2,$1)')')

define(`el', `
    /* $1 fields */define(`this', `$1')`'dnl
    body_fn_get(,,,field_first)`'dnl
    TThandy_$1($1,$1)`'dnl
    body_fn_get(,,,field_last)`'dnl
')
define(`extends')
define(`exported', `def_handy($@)')
define(`field', `wrap_export($@)')
define(`exported_fields', `TTdef_$1($1,$1)')

divert

/* array for field names */

static TT_CONST struct s_ttfield field_array_c[] = {
TTlist()
};


#define ttobj_field_n (sizeof(field_array_c)/sizeof(field_array_c[0]))

static struct s_ttfield field_array[ttobj_field_n];

define(`exported_fields')
define(`exported')
define(`extends')
define(`field')

