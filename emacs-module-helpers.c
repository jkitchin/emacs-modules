#include <string.h>
#include "emacs-module.h"
#include "emacs-module-helpers.h"

// Extract a number as a double from arg. ints are cast as floats.
double extract_double (emacs_env *env, emacs_value arg)
{
  emacs_value type = env->type_of(env, arg);
  double result;
  if (env->eq(env, type, env->intern(env, "integer")))
    {
      result = (float) env->extract_integer(env, arg);
    } else if (env->eq(env, type, env->intern(env, "float")))
    {
      result = env->extract_float(env, arg);
    }
  return result;
}

// Extract a number as an integer from arg. floats are cast as ints.
int extract_integer (emacs_env *env, emacs_value arg)
{
  emacs_value type = env->type_of(env, arg);
  int result;
  if (env->eq(env, type, env->intern(env, "integer")))
    {
      result = env->extract_integer(env, arg);
    } else if (env->eq(env, type, env->intern(env, "float")))
    {
      result = (int) env->extract_float(env, arg);
    }
  return result;
}

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
  emacs_value qlist = env->funcall(env, list, 4, largs);   

  // now eval the list of symbols
  emacs_value args[] = { qlist };  
  env->funcall(env, eval, 1, args);
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
