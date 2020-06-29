;;
;;  compile-file
;;
(defconstant +compile-input+ #p"_debug_input.lisp")
(defconstant +compile-output+ #p"_debug_output.fasl")
(defvar *result*)

;;
;;  reader-comma
;;
(defstruct reader-comma value)

(defun dispatch-sharp-comma (stream char number)
  (declare (ignore char number))
  (make-reader-comma :value (read stream nil nil t)))

(set-dispatch-macro-character #\# #\, #'dispatch-sharp-comma)

(defmethod print-object ((inst reader-comma) stream)
  (let ((value (reader-comma-value inst)))
    (format stream "#.")
    (write value :stream stream)))


;;
;;  delete-file
;;
(defun test-compile-delete ()
  (flet ((del (x) (when (probe-file x)
                    (delete-file x))))
    (del +compile-input+)
    (del +compile-output+)))


;;
;;  test
;;
(defun test-compile-load (x)
  (with-open-file (stream +compile-input+ :direction :output
                          :if-exists :supersede
                          :if-does-not-exist :create)
    (let ((*print-pretty* t)
          (*print-right-margin* 70))
      (format stream "~S~%" x)))
  (compile-file +compile-input+ :output-file +compile-output+)
  (setq *result* nil)
  (load +compile-output+)
  (prog1 *result*
    (setq *result* nil)))

(defun call-compile (x)
  (unwind-protect
    (test-compile-load x)
    (test-compile-delete)))

(defmacro test-compile (x)
  `(call-compile ',x))

(defmacro value-compile (x)
  `(call-compile '(setq *result* ,x)))


;;
;;  type
;;
(defun type-compile-load (x)
  (with-open-file (stream +compile-input+ :direction :output
                          :if-exists :supersede
                          :if-does-not-exist :create)
    (let ((*print-pretty* t)
          (*print-right-margin* 70))
      (format stream "(setq *result* #.(lisp-system::parse-type '~S))" x)))
  (compile-file +compile-input+ :output-file +compile-output+)
  (setq *result* nil)
  (load +compile-output+)
  (prog1 *result*
    (setq *result* nil)))

(defun calltype-compile (x)
  (unwind-protect
    (lisp-system::type-object
      (type-compile-load x))
    (test-compile-delete)))

(defmacro type-compile (x)
  `(calltype-compile ',x))


;;
;;  exec
;;
(defun exec-compile-load (x)
  (with-open-file (stream +compile-input+ :direction :output
                          :if-exists :supersede
                          :if-does-not-exist :create)
    (let ((*print-pretty* t)
          (*print-right-margin* 70))
      (format stream "(setq *result* #.~S)" x)))
  (compile-file +compile-input+ :output-file +compile-output+)
  (setq *result* nil)
  (load +compile-output+)
  (prog1 *result*
    (setq *result* nil)))

(defun exec-call-compile (x)
  (unwind-protect
    (exec-compile-load x)
    (test-compile-delete)))

(defmacro exec-compile (x)
  `(exec-call-compile ',x))


;;
;;  expr
;;
(defun expr-compile-load (x)
  (with-open-file (stream +compile-input+ :direction :output
                          :if-exists :supersede
                          :if-does-not-exist :create)
    (let ((*print-pretty* t)
          (*print-right-margin* 70))
      (format stream "(setq *result* ~S)" x)))
  (compile-file +compile-input+ :output-file +compile-output+)
  (setq *result* nil)
  (load +compile-output+)
  (prog1 *result*
    (setq *result* nil)))

(defun expr-call-compile (x)
  (unwind-protect
    (expr-compile-load x)
    (test-compile-delete)))

(defmacro expr-compile (x)
  `(expr-call-compile ',x))


;;
;;  file compile
;;
(defun test-file-compile-list (args)
  (with-open-file (stream +compile-input+ :direction :output
                          :if-exists :supersede
                          :if-does-not-exist :create)
    (let ((*print-pretty* t)
          (*print-right-margin* 70))
      (dolist (x args)
        (format stream "~S~%" x))))
  (compile-file +compile-input+ :output-file +compile-output+))

(defmacro test-file-compile (&rest args)
  `(test-file-compile-list ',args))

(defun test-file-load ()
  (load +compile-output+))

(defun test-file-compile-string (&rest args)
  (with-open-file (stream +compile-input+ :direction :output
                          :if-exists :supersede
                          :if-does-not-exist :create)
    (dolist (x args)
      (format stream "~A~%" x)))
  (compile-file +compile-input+ :output-file +compile-output+))

