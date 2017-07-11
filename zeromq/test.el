(progn
  (add-to-list 'load-path (expand-file-name "."))
  (require 'mod-zmq)
  (zmq-send-and-recv "Hello"))

(stest "there")


(defun test-1 ()
  (print (zmq-send-and-recv "Hello"))
  (print (zmq-send-and-recv "Hello You")))
