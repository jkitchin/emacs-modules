#include <gsl/gsl_const_mksa.h>
#include "emacs-module.h"
#include "emacs-module-helpers.h"

int plugin_is_GPL_compatible;

int emacs_module_init(struct emacs_runtime *ert)
{
  emacs_env *env = ert->get_environment(ert);

  defconst(env, "GSL-CONST-MKSA-SPEED-OF-LIGHT",
	   GSL_CONST_MKSA_SPEED_OF_LIGHT,
	   "Speed of light in vacuum (m/s).");
  
  defconst(env, "GSL-CONST-MKSA-PLANCKS-CONSTANT-H",
	   GSL_CONST_MKSA_PLANCKS_CONSTANT_H,
	   "Plank's constant, h");

  provide(env, "gsl-constants");
  
  return 0;
}
