(add-to-list 'load-path (expand-file-name "."))


(defun test-linalg () 
  (require 'gsl-linalg)
  (print (gsl-linalg-LU-solve
	  [[0.18 0.60 0.57 0.96]
	   [0.41 0.24 0.99 0.58]
	   [0.14 0.30 0.97 0.66]
	   [0.51 0.13 0.19 0.85]]
	  [1.0 2.0 3.0 4.0]))
  (print (gsl-linalg-LU-solve
	  [[0.18 0.60 0.57 0.96]
	   [0.41 0.24 0.99 0.58]
	   [0.14 0.30 0.97 0.66]
	   [0.51 0.13 0.19 0.85]]
	  [1.0 2.0 3.0 4]))

  (print (gsl-blas-dgemm
	  [[0.11 0.12 0.13]
	   [0.21 0.22 0.23]]
	  [[1011 1012]
	   [1021 1022]
	   [1031 1032]])))


(defun test-mkl ()
  (let ((process-environment )))
  (add-to-list 'process-environment "LD_LIBRARY_PATH=/opt/intel/compilers_and_libraries_2017.4.181/mac/mkl/lib")
  
  (add-to-list 'process-environment  "DYLD_LIBRARY_PATH=/opt/intel/compilers_and_libraries_2017.4.181/mac/tbb/lib:/opt/intel/compilers_and_libraries_2017.4.181/mac/compiler/lib:/opt/intel/compilers_and_libraries_2017.4.181/mac/mkl/lib")
  (require 'mod-mkl)

  (print (getenv "LD_LIBRARY_PATH"))
  
  (print (mkl-dgemm
	  [[0.11 0.12 0.13]
	   [0.21 0.22 0.23]]
	  [[1011 1012]
	   [1021 1022]
	   [1031 1032]])))


(defun test-constants ()
  (require 'gsl-constants)
  (print GSL-CONST-MKSA-PLANCKS-CONSTANT-H)
  (print (describe-variable 'GSL-CONST-MKSA-PLANCKS-CONSTANT-H)))


(defun test-types ()
  (require 'mod-types)
  (print (mt 1))
  (print (mt 2.0))
  (print (mtype 0))
  (print (mtype 0.0))
  (print (mtype "0.0"))
  (print (mtype '(0.0 1.0)))
  (print (mtype [0.0]))
  (print (mtype (current-buffer)))
  (print (mtype (make-hash-table)))
  (print (mtype 'test))
  (print (mvariadic))
  (print (mvariadic 1))
  (print (mvariadic 1 2))
  (print (mvariadic 1 2 3))
  (print (mvariadic 1 2 3 4)))


(defun test-integration ()
  (require 'gsl-integration)
  (print (gsl-integration-qags (lambda (x params) (/ (log x) (sqrt x))) 0.0 1.0))
  ;; integer test
  (print (gsl-integration-qags (lambda (x params) (/ (log x) (sqrt x))) 0 1))
  (print (gsl-integration-qags (lambda (x params) (/ (log x) (sqrt x))) 0.0 1.0 nil nil 0.01 500))
  (print (gsl-integration-qags (lambda (x params) (/ (log x) (sqrt x))) 0.0 1.0 nil nil 0.01 500.0))
  (print (gsl-integration-qags (lambda (x params) (car params)) 0.0 1.0 '(0.5))))


(defun test-roots ()
  (require 'gsl-roots)
  (print (gsl-root-fsolver-brent (lambda (x params) (- (* x x) 5)) 0.0 5.0))
  (print (gsl-root-fsolver-brent (lambda (x params) (- (* x x) 5)) 0 5))
  (print (gsl-root-fsolver-brent (lambda (x params) (- (* x x) 5)) 0.0 5.0 nil nil 1e-6))) 


(defun test-list-vec ()
  (require 'mod-list-vec)
  (print (f1 3))
  (print (f2 [10 9]))
  (print (f3))
  (print (f4 [1 2] 2))
  (prin1 "4a list ")
  (print (f4a '(1 2) 3))
  (prin1 "4a vec ")
  (print (f4a [3 4] 3))
  (print (f5 [3 4 5]))
  (print (f6 [[1 2] [3 4]]))
  (print (f7 2 '(4 3 2 1))))
