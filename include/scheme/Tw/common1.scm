
;; this file should be executed as
;;   chezscheme --script include/scheme/Tw/common1.scm > include/Tw/common1_gen.h

(define (F_EASY name str)
  (display "#define Tw")
  (display name)
  (display " Tw_")
  (display name)
  (newline))

(define (F_DECL ret name args)
  (display "#define Tw")
  (display name)
  (display "(")
  (do ((l args (cdr l)))
      ((null? l))
    (let* ((arg (car l))
           (name (cadr arg)))
      (unless (eq? l args)
        (display ", "))
      (display name)))
  (display ")\tTw_")
  (display name)
  (display "(Tw_DefaultD")
  (do ((l args (cdr l)))
      ((null? l))
    (let* ((arg (car l))
           (name (cadr arg)))
      (display ", ")
      (display name)))
  (display ")\n"))


(include "include/scheme/Tw/common_inc.scm")
