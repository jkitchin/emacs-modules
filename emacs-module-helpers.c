#include <string.h>
#include "emacs-module.h"
#include "emacs-module-helpers.h"

void defconst (emacs_env *env, const char *name, double value, const char *doc)
{
  // These are functions we will call
  emacs_value eval = env->intern(env, "eval");  
  emacs_value list = env->intern(env, "list");

  // These will make up the list we will eventally eval
  emacs_value fdefconst = env->intern(env, "defconst");
  emacs_value sym = env->intern(env, name);
  emacs_value val = env->make_float(env, value);
  emacs_value sdoc = env->make_string(env, doc, strlen(doc));

  // make a list of (defconst sym val doc)
  emacs_value largs[] = {fdefconst, sym, val, sdoc};
  emacs_value qlist = env->funcall(env, list, 4, &largs);   

  // now eval the list of symbols
  emacs_value args[] = { qlist };  
  env->funcall(env, eval, 1, &args);
}

void bind_function (emacs_env *env, const char *name, emacs_value Sfun)
{
  /* Set the function cell of the symbol named NAME to SFUN using
     the 'fset' function.  */

  /* Convert the strings to symbols by interning them */
  emacs_value Qfset = env->intern (env, "fset");
  emacs_value Qsym = env->intern (env, name);

  /* Prepare the arguments array */
  emacs_value args[] = { Qsym, Sfun };

  /* Make the call (2 == nb of arguments) */
  env->funcall (env, Qfset, 2, args);
}


void provide (emacs_env *env, const char *feature)
{
  emacs_value Qfeat = env->intern (env, feature);
  emacs_value Qprovide = env->intern (env, "provide");
  emacs_value args[] = { Qfeat };

  env->funcall (env, Qprovide, 1, args);
}
