

;; this file should be executed as
;;   chezscheme --script include/scheme/Tw/common.scm > include/Tw/common_gen.h

(define (c_verbatim str)
  (display str)
  (newline))

(define (F_EASY name str)
  (display str)
  (newline))

(define (TYPE_DECL type)
  (if (pair? type)
    (do ((l type (cdr l)))
        ((null? l))
      (display (car l))
      (display #\space))
    (begin
      (display type)
      (display #\space))))

(define (F_DECL ret name args)
  (TYPE_DECL ret)
  (display "Tw_")
  (display name)
  (display "(tdisplay TwD")
  (do ((l args (cdr l)))
      ((null? l))
    (let* ((arg (car l))
           (type (car arg))
           (name (cadr arg)))
      (display ", ")
      (TYPE_DECL type)
      (display name)))
  (display ");\n"))


(include "include/scheme/Tw/common_inc.scm")
