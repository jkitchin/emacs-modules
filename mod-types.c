#include "emacs-module.h"
#include "emacs-module-helpers.h"
#include <string.h>
int plugin_is_GPL_compatible;

// adapted from https://github.com/syohex/emacs-mruby-test/blob/master/mruby-core.c
static bool eq_type(emacs_env *env, emacs_value type, const char *type_str)
{
	return env->eq(env, type, env->intern(env, type_str));
}

// Function to return arg * 2 where arg can be an integer or float
static emacs_value mt (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  emacs_value arg = args[0];
  emacs_value type = env->type_of(env, arg);

  double a;
  if (eq_type(env, type, "integer"))
    {
      a = (float) env->extract_integer(env, arg);      
    } else if (eq_type(env, type, "float"))
    {
      a = env->extract_float(env, arg);
      
    }
  return env->make_float(env, 2.0 * a);
}

// return the type of the arg in a formatted string
static emacs_value mtype (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  emacs_value format = env->intern(env, "format");
  char *f = "%S is a %s";
  
  emacs_value fargs[] = {
    env->make_string(env, f, strlen(f)),
    args[0],
    env->type_of(env, args[0])
  };
  
  return env->funcall(env, format, 3, fargs);
}

// This is a variadic function that just prints the number of arguments it got.
static emacs_value mvariadic (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  return env->make_integer(env, nargs);
}

int emacs_module_init(struct emacs_runtime *ert)
{
  emacs_env *env = ert->get_environment(ert);

  DEFUN("mt", mt, 1, 1,
	"(mt arg) multiply arg by 2.", NULL);

  DEFUN("mtype", mtype, 1, 1, "Return the arg type.", NULL);
  provide(env, "mod-types");

  // use -2 for maxargs to make it variadic
  DEFUN("mvariadic", mvariadic, 0, -2, "variadic function.", NULL);
  provide(env, "mod-types");
  
  return 0;
}
