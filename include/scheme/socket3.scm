

;; this file should be executed as
;;   chezscheme --script include/scheme/socket3.scm > server/socket3_gen.h


(define (c_doxygen str)
  (if #f #f))


(define (ARG_ARRAY? arg)
  (let ((kind (cadr arg)))
    (pair? kind)))


(define (ARGS_ARRAY? args)
  (find ARG_ARRAY? args))


(define (ARG_ARRAY_FACTOR f)
  (cond
    ((pair? f)
      (display " a[")
      (display (cadr f))
      (display "]_any"))
    (else
      (display " ")
      (display f))))

(define (ARG_ARRAY arg i)
  (let ((kind (cadr arg)))
    (display "      case ")
    (display i)
    (display ": L =")
    (for-each ARG_ARRAY_FACTOR (cdr kind))
    (display "; break;\n")))


(define (ARGS_ARRAY args)
  (do ((i 1 (+ i 1))
       (l args (cdr l)))
      ((null? l))
    (let ((arg (car l)))
      (when (ARG_ARRAY? arg)
        (ARG_ARRAY arg i)))))


(define (FUNC_NAME func)
  (display (car func))
  (display (cadr func)))


(define (F_PROTO ret func args)
  (when (ARGS_ARRAY? args)
    (display "\n  case order_")
    (FUNC_NAME func)
    (display ":\n    switch (n) {\n")
    (ARGS_ARRAY args)
    (display "    }\n    break;\n")))


(define F_PROTOFindFunction F_PROTO)
(define F_PROTOSyncSocket   F_PROTO)

(display "/* clang-format off */\n")

(include "include/scheme/sockproto_inc.scm")
