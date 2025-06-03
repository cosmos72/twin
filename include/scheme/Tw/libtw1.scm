

;; this file should be executed as
;;   chezscheme --script include/scheme/Tw/libtw1.scm > libs/libtw/libtw1_gen.h


(define (c_doxygen str)
  (if #f #f))


(define (TYPE_DECL type kind)
  (display " \"")
  (display (if (pair? kind) (car kind) kind))
  (display "\" TWS_")
  (display (if (or (eq? 'x kind) (and (pair? kind) (memq (car kind) '(X Y))))
             "tobj"
             type))
  (display "_STR"))


(define (ARG_DECL arg)
  (TYPE_DECL (car arg) (cadr arg)))

(define (ARGS_DECL args)
  (do ((l args (cdr l)))
      ((null? l))
    (ARG_DECL (car l))))


(define (FUNC_NAME func)
  (display "Tw_")
  (display (car func))
  (display (cadr func)))

(define (FUNC_NAMELEN func)
  (display (+ (string-length (symbol->string (car func)))
              (string-length (symbol->string (cadr func))))))


(define (F_PROTO ret func args)
  (display "{ ")
  (FUNC_NAME func)
  (display ", ")
  (FUNC_NAMELEN func)
  (display ",\n")
  (display (* 2 (+ 1 (length args))))
  (display ", \"")
  (FUNC_NAME func)
  (display "\", \"")
  (display (caddr func))
  (display "\"")
  (TYPE_DECL (car ret) (cadr ret))
  (ARGS_DECL args)
  (display " },\n"))


(define F_PROTOFindFunction F_PROTO)
(define F_PROTOSyncSocket   F_PROTO)

(include "include/scheme/sockproto_inc.scm")
