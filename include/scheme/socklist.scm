

;; this file should be executed as
;;   chezscheme --script include/scheme/socklist.scm > include/socklist_gen.h

(display "/* clang-format off */\n")
(display "\n")
(display "#define OK_MAGIC      ((uldat)0x3E4B4F3Cul)\n")
(display "#define FAIL_MAGIC    ((uldat)0xFFFFFFFFul)\n")
(display "#define FIND_MAGIC    ((uldat)0x646E6946ul) /* i.e. \"Find\" */\n")
(display "#define MSG_MAGIC     ((uldat)0x2167734dul) /* i.e. \"Msg!\" */\n")
(display "\n")


(define (c_doxygen str)
  (if #f #f))


(define (FUNC_NAME func)
  (display (car func))
  (display (cadr func)))


(define (F_PROTO ret func args)
  (display "EL(")
  (FUNC_NAME func)
  (display ")\n"))


(define F_PROTOFindFunction F_PROTO)
(define F_PROTOSyncSocket   F_PROTO)

(include "include/scheme/sockproto_inc.scm")
