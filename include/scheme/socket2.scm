

;; this file should be executed as
;;   chezscheme --script include/scheme/socket2.scm > server/socket2_gen.h


(define (c_doxygen str)
  (if #f #f))


(define (TYPE_STR type kind)
  (display " \"")
  (display (if (pair? kind) (car kind) kind))
  (cond
    ((or (eq? 'x kind) (and (pair? kind) (memq (car kind) '(X Y))))
      (display "\" T")
      (display type)
      (display "_magic_STR"))
    (else
      (display "\" TWS_")
      (display type)
      (display "_STR"))))


(define (ARG_STR arg)
  (TYPE_STR (car arg) (cadr arg)))


(define (ARGS_STR args)
  (do ((l args (cdr l)))
      ((null? l))
    (ARG_STR (car l))))


(define (FUNC_NAME func)
  (display (car func))
  (display (cadr func)))


(define (F_PROTO ret func args)
  (display "\n{ 0, 0, \"")
  (FUNC_NAME func)
  (display "\",\n  \"")
  (display (caddr func))
  (display "\"")
  (ARGS_STR (cons ret args))
  (display " },\n"))


(define F_PROTOFindFunction F_PROTO)
(define F_PROTOSyncSocket   F_PROTO)

(display "/* clang-format off */\n")

(include "include/scheme/sockproto_inc.scm")
