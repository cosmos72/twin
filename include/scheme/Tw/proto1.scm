

;; this file should be executed as
;;   chezscheme --script include/scheme/Tw/proto1.scm > include/Tw/proto1_gen.h

(define (c_doxygen str)
  (display str)
  (newline))

(define (ARG_DECL arg)
  (display (caddr arg)))

(define (ARGS_DECL args)
  (display "(")
  (do ((l args (cdr l)))
      ((null? l))
    (unless (eq? l args)
      (display ", "))
    (ARG_DECL (car l)))
  (display ")"))

(define (FUNC_NAME func)
  (display (car func))
  (unless (eq? 'ChangeField (car func))
    (display (cadr func))))

(define (F_PROTO ret func args)
  ;; left side of C macro
  (display "#define Tw")
  (FUNC_NAME func)
  (ARGS_DECL args)
  ;; right side of C macro
  (display "\tTw_")
  (FUNC_NAME func)
  (ARGS_DECL (cons '(void x Tw_DefaultD) args))
  (display "\n"))

(define F_PROTOFindFunction F_PROTO)
(define F_PROTOSyncSocket   F_PROTO)

(include "include/scheme/sockproto_inc.scm")
