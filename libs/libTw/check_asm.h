
/*
 * disable i386 gcc assembler functions
 * if (tlargest) does not fit a machine register
 */
#ifdef TW_HAVE_GCC_I386_ASM
# if TW_SIZEOFTLARGEST > 4
#  undef TW_HAVE_GCC_I386_ASM
# endif
#endif

