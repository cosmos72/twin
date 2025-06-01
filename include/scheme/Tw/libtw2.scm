

;; this file should be executed as
;;   chezscheme --script include/scheme/Tw/libtw2.scm > libs/libtw/libtw2_gen.h


(define (TYPE_DECL type kind)
  (when (pair? kind)
    (display "const "))
  (when (or (eq? 'x kind) (and (pair? kind) (memq (car kind) '(X Y))))
    (display "t"))
  (display type)
  (if (pair? kind)
    (display " *")
    (display #\space)))


(define (ARG_DECL arg i)
  (TYPE_DECL (car arg) (cadr arg))
  (cond
    ((zero? i)
      (display (caddr arg)))
    (else
      (display "a")
      (display i))))


(define (ARGS_DECL args)
  (display "(")
  (do ((i 0 (+ i 1))
       (l args (cdr l)))
      ((null? l))
    (unless (zero? i)
      (display ", "))
    (ARG_DECL (car l) i))
  (display ")"))


(define (ARG_ARRAY_CALL type expr)
  (display "n((")
  (do ((l (cdr expr) (cdr l)))
      ((null? l))
    (unless (eq? l (cdr expr))
      (display #\space))
    (let ((factor (car l)))
      (cond
        ((pair? factor) ; (A NNN)
          (display "a")
          (display (cadr factor)))
        (else
          (display factor)))))
  (display ") * sizeof(")
  (TYPE_DECL type (if (memq (car expr) '(X y)) 'x '_))
  (display "))"))


(define (ARG_CALL arg i)
  (cond
    ((zero? i)
      (display (caddr arg)))
    ((pair? (cadr arg))
      (ARG_ARRAY_CALL (car arg) (cadr arg))
      (display ", N(a")
      (display i)
      (display ")"))
    (else
      (display "n(a")
      (display i)
      (display ")"))))


(define (ARGS_CALL args)
  (do ((i 0 (+ i 1))
       (l args (cdr l)))
      ((null? l))
    (display ", ")
    (ARG_CALL (car l) i)))


(define (FUNC_NAME func)
  (display (car func))
  (unless (eq? 'ChangeField (car func))
    (display (cadr func))))


(define (FUNC_DEFINE ret func args flag)
  (let ((args (cons '(tw_d _ TwD) args)))
    (TYPE_DECL (car ret) (cadr ret))
    (display (if (symbol? flag) "_Tw_" "Tw_"))
    (FUNC_NAME func)
    (ARGS_DECL args)
    (cond
      ((eq? 'void (car ret))
        (display " {\n  _Tw_EncodeCall("))
      (else
        (display " {\n  return (")
        (TYPE_DECL (car ret) (cadr ret))
        (display ")_Tw_EncodeCall(")))
    (display
      (cond
        ((symbol? flag)        flag)
        ((eq? 'void (car ret)) 'ENCODE_FL_VOID)
        (else                  0)))
    (display ", order_")
    (display (car func))
    (display (cadr func))
    (ARGS_CALL args)
    (display ");\n}\n\n")))


(define (F_PROTO ret func args)
  (FUNC_DEFINE ret func args 0))

(define (F_PROTOFindFunction ret func args)
  (display "static ")
  (FUNC_DEFINE ret func args 'ENCODE_FL_NOLOCK))

(define (F_PROTOSyncSocket ret func args)
  (display "static ")
  (FUNC_DEFINE ret func args 'ENCODE_FL_NOLOCK))

(display "#define n(a) ((tany)a)\n")
(display "#define N(a) ((const void *)a)\n")

(include "include/scheme/sockproto_inc.scm")

(display "#undef n\n")
(display "#undef N\n")
