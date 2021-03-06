inline int lrawkbd_test_bit(uldat nr, void *addr_) {
  uldat mask, *addr = (uldat *)addr_;

  addr += nr / BITS_PER_ULDAT;
  mask = 1 << (nr & (BITS_PER_ULDAT - 1));
  return ((mask & *addr) != 0);
}

inline void lrawkbd_set_bit(uldat nr, void *addr_) {
  uldat mask, *addr = (uldat *)addr_;

  addr += nr / BITS_PER_ULDAT;
  mask = 1 << (nr & (BITS_PER_ULDAT - 1));

  *addr |= mask;
}

inline int lrawkbd_test_and_set_bit(uldat nr, void *addr_) {
  uldat mask, *addr = (uldat *)addr_;
  int retval;

  addr += nr / BITS_PER_ULDAT;
  mask = 1 << (nr & (BITS_PER_ULDAT - 1));

  retval = (mask & *addr) != 0;
  *addr |= mask;
  return retval;
}

inline void lrawkbd_clear_bit(uldat nr, void *addr_) {
  uldat mask, *addr = (uldat *)addr_;

  addr += nr / BITS_PER_ULDAT;
  mask = 1 << (nr & (BITS_PER_ULDAT - 1));

  *addr &= ~mask;
}

inline int lrawkbd_test_and_clear_bit(uldat nr, void *addr_) {
  uldat mask, *addr = (uldat *)addr_;
  int retval;

  addr += nr / BITS_PER_ULDAT;
  mask = 1 << (nr & (BITS_PER_ULDAT - 1));

  retval = (mask & *addr) != 0;
  *addr &= ~mask;
  return retval;
}
