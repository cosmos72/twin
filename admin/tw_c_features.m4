m4_define([TW_C_FEATURES], [

AC_CACHE_CHECK(for gcc-compatible 'static inline', ac_cv_c_static_inline, AC_TRY_COMPILE( [
#undef inline
  static inline int ret0(void) {
    return 0;
  }
] , [
  int a = ret0();
],
  ac_cv_c_static_inline=yes,
  ac_cv_c_static_inline=no))

AC_CACHE_CHECK(for gcc-compatible 'attribute((const))', ac_cv_c_attribute_const, AC_TRY_COMPILE( [
#undef __attribute__
  static int ret0(void) __attribute__((const));
  static int ret0(void) {
    return 0;
  }
] , [
  int a = ret0();
],
  ac_cv_c_attribute_const=yes,
  ac_cv_c_attribute_const=no))

AC_CACHE_CHECK(for gcc-compatible 'attribute((packed))', ac_cv_c_attribute_packed, AC_TRY_COMPILE( , [
#undef __attribute__
  struct sp {
    int a;
    char b;
  } __attribute__((packed));
  struct sp s[2];
],
  ac_cv_c_attribute_packed=yes,
  ac_cv_c_attribute_packed=no))

AC_CACHE_CHECK(for gcc-compatible i386 assembler, ac_cv_prog_gcc_i386_asm, AC_TRY_COMPILE( , [
#ifdef __i386__
#undef asm
  asm ("\n\tmovl %eax,%eax"
       "\n\tpushl %ecx"
       "\n\tpushl $1"
       "\n\tpopl %ecx"
       "\n\tpopl %ecx");
#else
  choke me
#endif
],
  ac_cv_prog_gcc_i386_asm=yes,
  ac_cv_prog_gcc_i386_asm=no))

])
