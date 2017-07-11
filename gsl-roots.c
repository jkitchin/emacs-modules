#include <gsl/gsl_errno.h>
#include <gsl/gsl_roots.h>
#include "emacs-module.h"
#include "emacs-module-helpers.h"

int plugin_is_GPL_compatible;

static emacs_value Fgsl_root_fsolver_brent (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{

// nested function - only supported as an extension in gcc
  double f (double x, void *params) 
  {
    emacs_value fn = args[0];  // function we will integrate
    emacs_value x2[] = { env->make_float(env, x), params };
    emacs_value y = env->funcall(env, fn, 2, x2);   
    
    return env->extract_float (env, y);
  }

  int status;
  int iter = 0, max_iter = 100;
  const gsl_root_fsolver_type *T;
  gsl_root_fsolver *s;
  double result = 0;

  double x_lo = extract_double (env, args[1]);
  double x_hi = extract_double (env, args[2]);

 gsl_function F;

  F.function = &f;
  if (nargs >= 4) {F.params = args[3];}

  // default values for optional arguments
  double epsabs = 0.0;
  double epsrel = 0.001;

  if (nargs >= 5 && env->is_not_nil(env, args[4])) {epsabs = extract_double(env, args[4]);}
  if (nargs >= 6 && env->is_not_nil(env, args[5])) {epsrel = extract_double(env, args[5]);}
  
  T = gsl_root_fsolver_brent;
  s = gsl_root_fsolver_alloc (T);
  gsl_root_fsolver_set (s, &F, x_lo, x_hi);

  do
    {
      iter++;
      status = gsl_root_fsolver_iterate (s);
      result = gsl_root_fsolver_root (s);
      x_lo = gsl_root_fsolver_x_lower (s);
      x_hi = gsl_root_fsolver_x_upper (s);
      status = gsl_root_test_interval (x_lo, x_hi,
                                       epsabs, epsrel);

    }
  while (status == GSL_CONTINUE && iter < max_iter);

  gsl_root_fsolver_free (s);

  return env->make_float (env, result);
}

int emacs_module_init(struct emacs_runtime *ert)
{
  emacs_env *env = ert->get_environment(ert);
  
  DEFUN("gsl-root-fsolver-brent", Fgsl_root_fsolver_brent, 3, 7,
	"(gsl-root-fsolver-brent f xlo xhi params &optional epsabs epsrel)\n" \
	"Solve f(x; params) = 0 using the Brent method.\n" \
	"https://www.gnu.org/software/gsl/doc/html/roots.html#examples",
	0);
  provide(env, "gsl-roots");
  
  return 0;
}
