(progn
  (add-to-list 'load-path (expand-file-name "."))
  (require 'mod-zmq)
  

  (let* ((context (zmq-ctx-new))
	 (requester (zmq-socket context ZMQ-REQ)))

    (zmq-connect requester "tcp://localhost:5555")
    (zmq-send requester "test" 0)

    (message "recv: %s" (zmq-recv requester 10 0))
    
    (prog1
	(print (zmq-close requester))
      (zmq-ctx-destroy context))))


(defun test-1 ()
  (let* ((context (zmq-ctx-new))
	 (requester (zmq-socket context ZMQ-REQ)))

    (zmq-connect requester "tcp://localhost:5555")
    (zmq-send requester "test-2" 0)

    (message "recv: %s" (zmq-recv requester 10 0))
    
    (prog1
	(print (zmq-close requester))
      (zmq-ctx-destroy context))))
