m4_define([TW_C_FEATURES], [

AC_CACHE_CHECK(for volatile, ac_cv_c_volatile, AC_TRY_COMPILE( [
#undef volatile
  int deref(int volatile * i) {
    return *i;
  }
] , [
  volatile int a = 3;
  int b = deref(&a);
],
  ac_cv_c_volatile=yes,
  ac_cv_c_volatile=no))

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
  ac_cv_c_attribute_const='__attribute__((const))',
  ac_cv_c_attribute_const=no))

AC_CACHE_CHECK(for gcc-compatible 'attribute((packed))', ac_cv_c_attribute_packed, AC_TRY_COMPILE( [
#undef __attribute__
  struct sp {
    char a;
    int b;
  } __attribute__((packed));
], [[
  struct sp s[2];
  char compare_sizeof_packed_struct[ sizeof(struct sp) == sizeof(char) + sizeof(int) ? 1 : -1 ];
]],
  ac_cv_c_attribute_packed='__attribute__((packed))',
  ac_cv_c_attribute_packed=no))

AC_CACHE_CHECK(for gcc-compatible 'attribute((aligned(1)))', ac_cv_c_attribute_aligned_1, AC_TRY_COMPILE( [
], [[
#undef __attribute__
  char buf[2 * sizeof(int)];
  int * __attribute__((aligned(1))) p = buf + 1;
  *(int * __attribute__((aligned(1))))p = 0;
]],
  ac_cv_c_attribute_aligned_1='__attribute__((aligned(1)))',
  ac_cv_c_attribute_aligned_1=no))

AC_CACHE_CHECK(for gcc-compatible i386 assembler, ac_cv_prog_gcc_i386_asm, AC_TRY_COMPILE( [
], [
#ifdef __i386__
#undef asm
  asm ("\n\tmovl %eax,%eax"
       "\n\tpushl %ecx"
       "\n\tpushl [$]1"
       "\n\tpopl %ecx"
       "\n\tpopl %ecx");
#else
  choke me
#endif
],
  ac_cv_prog_gcc_i386_asm=yes,
  ac_cv_prog_gcc_i386_asm=no))

])
