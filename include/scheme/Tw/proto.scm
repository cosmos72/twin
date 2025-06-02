

;; this file should be executed as
;;   chezscheme --script include/scheme/Tw/proto.scm > include/Tw/proto_gen.h

(define (c_doxygen str)
  (display str)
  (newline))

(define (TYPE_DECL type kind)
  (when (pair? kind)
    (display "const "))
  (when (or (eq? 'x kind) (and (pair? kind) (eq? 'X (car kind))))
    (display "t"))
  (display type)
  (if (pair? kind)
    (display " *")
    (display #\space)))

(define (ARG_DECL arg)
  (TYPE_DECL (car arg) (cadr arg))
  (let ((name (caddr arg)))
    (display name)))

(define (F_PROTO ret func args)
  (TYPE_DECL (car ret) (cadr ret))
  (display "Tw_")
  (display (car func))
  (unless (eq? 'ChangeField (car func))
    (display (cadr func)))
  (display "(tdisplay TwD")
  (do ((l args (cdr l)))
      ((null? l))
    (display ", ")
    (ARG_DECL (car l)))
  (display ");\n"))

(define F_PROTOFindFunction F_PROTO)
(define F_PROTOSyncSocket   F_PROTO)

(include "include/scheme/sockproto_inc.scm")
