#include "emacs-module.h"

#ifndef EMACS_MODULE_HELPERS_H_
#define EMACS_MODULE_HELPERS_H_

void defconst (emacs_env *env, const char *name, double value, const char *doc);

// I could not figure out how to define this as a function, so we use this define instead. 
#define DEFUN(lsym, csym, amin, amax, doc, data) \
  bind_function (env, lsym, \
		 env->make_function (env, amin, amax, csym, doc, data))

void provide (emacs_env *env, const char *feature);

#endif // EMACS_MODULE_HELPERS_H_
