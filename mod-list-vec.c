#include "emacs-module.h"
#include "emacs-module-helpers.h"
#include <stdlib.h>

/* Declare mandatory GPL symbol.  */
int plugin_is_GPL_compatible;

// This just returns the argument, works for a list.
static emacs_value Ff1 (emacs_env *env, int nargs, emacs_value args[], void *data)
{
  return args[0];
}

// get first element of a vector
static emacs_value Ff2 (emacs_env *env, int nargs, emacs_value args[], void *data)
{
  return env->vec_get(env, args[0], 0);
}

// This just returns a vector of integers!!!
static emacs_value Ff3 (emacs_env *env, int nargs, emacs_value args[], void *data)
{
 int len = 2;
 emacs_value *array = malloc(sizeof(emacs_value) * len);
 array[0] = env->make_integer(env, 2);
 array[1] = env->make_integer(env, 4);

 emacs_value Fvector = env->intern(env, "vector");
 emacs_value vec = env->funcall(env, Fvector, len, array);
 free(array);
 return vec;
}

// return vector * n
static emacs_value Ff4 (emacs_env *env, int nargs, emacs_value args[], void *data)
{
 emacs_value input = args[0];
 double N = extract_double(env, args[1]);

 int len = env->vec_size (env, input);

 emacs_value *array = malloc(sizeof(emacs_value) * len);

 // multiply each value by N
 for (ptrdiff_t i = 0; i < len; i++)
   {
     array[i] = env->make_float(env,
				N * extract_double(env,
						     env->vec_get (env, input, i)));
   }

 // If you change this to list, you get a list instead!
 emacs_value Fvector = env->intern(env, "vector");
 emacs_value vec = env->funcall(env, Fvector, len, array);
 free(array);
 return vec;
}

// get a vector or list in and return list * n
static emacs_value Ff4a (emacs_env *env, int nargs, emacs_value args[], void *data)
{
  emacs_value type = env->type_of(env, args[0]);
  double N = extract_double(env, args[1]);  

  emacs_value length = env->intern(env, "length");

  emacs_value lenargs[] = { args[0] }; 

  emacs_value elen = env->funcall(env, length, 1, lenargs);
 
  int len = env->extract_integer(env, elen);

  emacs_value *array = malloc(sizeof(emacs_value) * len);

  if (env->eq(env, type, env->intern(env, "cons")))
    {
      emacs_value nth = env->intern(env, "nth");
        
      for (int i = 0; i < len; i++)
	{
	  emacs_value nthargs[]  = { env->make_integer(env, i), args[0] };
	  double j = extract_double(env, env->funcall(env, nth, 2, nthargs));
	  array[i] = env->make_float(env, N * j);
	}

      emacs_value list = env->intern(env, "list");
      emacs_value L = env->funcall(env, list, len, array);
      free(array);
      return L;
    } else if (env->eq(env, type, env->intern(env, "vector")))
    {
      for (int i = 0; i < len; i++)
	{
	  array[i] = env->make_float(env,
				     N * extract_double(env,
							env->vec_get (env, args[0], i)));
	  }
      // If you change this to list, you get a list instead!
    }
  
 emacs_value list = env->intern(env, "list");
 emacs_value result = env->funcall(env, list, len, array);
 free(array);
 return result;
} 
  

// return 2nd element of vector
static emacs_value Ff5 (emacs_env *env, int nargs, emacs_value args[], void *data)
{
 emacs_value vec = args[0];

 return env->vec_get (env, vec, 1);
}

// get second value of second vector
static emacs_value Ff6 (emacs_env *env, int nargs, emacs_value args[], void *data)
{
 emacs_value vec = args[0];
 emacs_value v2 = env->vec_get (env, vec, 1);
 return env->vec_get (env, v2, 1);
}

// index a list
static emacs_value Ff7 (emacs_env *env, int nargs, emacs_value args[], void *data)
{
 emacs_value nth = env->intern(env, "nth");

 return env->funcall (env, nth, 2, args);
}

int emacs_module_init (struct emacs_runtime *ert)
{
  emacs_env *env = ert->get_environment (ert);

  DEFUN("f1", Ff1, 1, 1, "Return the argument", NULL);
  DEFUN("f2", Ff2, 1, 1, "return first element of a vector", NULL);
  DEFUN("f3", Ff3, 0, 0, "return a vector [2 4]", NULL);
  DEFUN("f4", Ff4, 2, 2, "return vector multiplied by arg", NULL);
  DEFUN("f4a", Ff4a, 2, 2, "return vector multiplied by arg", NULL);
  DEFUN("f5", Ff5, 1, 1, "return 2nd value of vector", NULL);
  DEFUN("f6", Ff6, 1, 1, "return 2nd value of 2d vector", NULL);
  DEFUN("f7", Ff7, 2, 2, "nth a list", NULL);

  provide (env, "mod-list-vec");

  /* loaded successfully */
  return 0;
}
