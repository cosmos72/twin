#!/usr/bin/env bash

SCHEME='chezscheme --script'

grep '^[0-9A-F]' < txt/EastAsianWidth.txt | \
  cut -b1-18 | grep '; [FW]' | cut -d';' -f1 | tr -d ' ' | \
  sed -E -e 's/^([0-9A-Fa-f]*)\.\.([0-9A-Fa-f]*)$/(#x\1 . #x\2)/' -e 's/^([0-9A-Fa-f]*)$/#x\1/' > txt/wide.scm

exec > libs/libtutf/wide.h

cat <<EOF
/*
 *  wcwidth_data.h  --  compute display width of UTF-32 (Unicode) characters
 *
 *  PLEASE DO NOT EDIT!
 *
 *  This file was automatically generated from http://unicode.org/Public/UNIDATA/EastAsianWidth.txt
 *  using the script twin/scripts/3-make-wcwidth-data.bash
 *
 *  All changes to this file will be lost when the script is re-executed.
 */

#ifndef TUTF_WIDE_H
#define TUTF_WIDE_H

EOF

$SCHEME /dev/stdin << EOF

(define w (make-bytevector #x40000 0))

(let ((in (open-input-file "txt/wide.scm")))
  (do ((obj (read in) (read in)))
      ((eof-object? obj))
    (if (pair? obj)
      (let ((lo (car obj))
            (hi (cdr obj)))
        (do ((i lo (fx1+ i)))
            ((fx>? i hi))
          (bytevector-u8-set! w i 1)))
      (bytevector-u8-set! w obj 1)))
  (close-port in))

(define narrow-fine '())
(define narrow-coarse '())
(define wide-fine '())
(define wide-coarse '())

(define (%append lo hi wide)
  (if (fxzero? wide)
    (if (fx>=? (fx- hi lo) 128)
      (set! narrow-coarse (cons (cons lo (fx1- hi)) narrow-coarse))
      (set! narrow-fine (cons (cons lo (fx1- hi)) narrow-fine)))
    (if (fx>=? (fx- hi lo) 128)
      (set! wide-coarse (cons (cons lo (fx1- hi)) wide-coarse))
      (set! wide-fine (cons (cons lo (fx1- hi)) wide-fine)))))

(let %loop ((lo 0)
            (hi 0)
            (wide 0))
  (if (fx<? hi #x20000)
    (if (fx=? wide (bytevector-u8-ref w hi))
      (%loop lo (fx1+ hi) wide)
      (begin
        (%append lo hi wide)
        (%loop hi hi (fx- 1 wide))))
    (%append lo hi wide)))

(define (%show-as-c-code l)
  (do ((tail l (cdr tail))
       (prefix "  return" "\n      ||"))
      ((null? tail))
    (let ((e (car tail)))
      (if (fxzero? (car e))
        (printf "~a r <= ~s" prefix (cdr e))
        (printf "~a (r >= ~s && r <= ~s)" prefix (car e) (cdr e))))))

(define (%show-as-c-data l)
  (do ((tail l (cdr tail)))
      ((null? tail))
    (let ((e (car tail)))
      (printf "{~s, ~s},\n" (car e) (cdr e)))))

(display "static inline unsigned is_wide_coarse(trune r) {\n")
(%show-as-c-code (reverse! wide-coarse))
(display ";\n}\n\n")

(display "static inline unsigned is_narrow_coarse(trune r) {\n")
(%show-as-c-code (reverse! narrow-coarse))
(display ";\n}\n\n")

(display "static const trune is_wide_fine[][2] = {\n")
(%show-as-c-data (reverse! wide-fine))
(display "};\n\n")

EOF

cat << EOF
#endif /* TUTF_WIDE_H */
EOF
