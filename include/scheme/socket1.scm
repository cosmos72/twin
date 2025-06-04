

;; this file should be executed as
;;   chezscheme --script include/scheme/socket1.scm > server/socket1_gen.h


(define (c_doxygen str)
  (if #f #f))

(define (RET_CAST type kind)
  (case kind
    ((_)
      (display "a[0]_any = (")
      (display type)
      (display ")"))
    ((x)
      (display "a[0]_obj = (Tobj)"))))

(define (TYPE_CAST type kind)
  (if (pair? kind)
    (display "(const ")
    (display "("))
  (when (or (eq? 'x kind) (and (pair? kind) (memq (car kind) '(X Y))))
    (display "T"))
  (display type)
  (if (pair? kind)
    (display " *)")
    (display ")")))


(define (ARG_CALL arg i)
  (let ((kind (cadr arg)))
    (TYPE_CAST (car arg) kind)
    (display "a[")
    (display i)
    (cond
      ((eq? 'x kind)
        (display "]_obj"))
      ((pair? kind)
        (display "]_vec"))
      (else
        (display "]_any")))))


(define (ARGS_CALL func_flag args)
  (let ((start (if (eqv? 2 func_flag) 2 1))
        (args  (if (eqv? 2 func_flag) (cdr args) args)))
    (do ((i start (+ i 1))
         (l args (cdr l)))
        ((null? l))
      (unless (eq? l args)
        (display ", "))
      (ARG_CALL (car l) i))))


(define (FUNC_NAME func)
  (display (car func))
  (display (cadr func)))


(define (F_PROTO ret func args)
  (display "case order_")
  (FUNC_NAME func)
  (display ":\n    if (a.size() >= ")
  (display (length args))
  (display ") {\n        ")
  (RET_CAST (car ret) (cadr ret))
  (case (caddr func)
    ((1)
      (display "S")
      (display (string-downcase (symbol->string (cadr func))))
      (display "::")
      (display (car func)))
    ((2)
      (let* ((arg1  (car args))
             (type1 (car arg1)))
        (display "((T")
        (display type1)
        (display ")a[1]_obj)->")
        (display (car func))))
    (else
      (display "sock")
      (FUNC_NAME func)))
  (display "(")
  (ARGS_CALL (caddr func) args)
  (display ");\n        return ttrue;\n    }\n    break;\n\n"))


(define F_PROTOFindFunction F_PROTO)
(define F_PROTOSyncSocket   F_PROTO)

(display "/* clang-format off */\n")

(include "include/scheme/sockproto_inc.scm")
