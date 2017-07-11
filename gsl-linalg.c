#include "emacs-module.h"
#include "emacs-module-helpers.h"
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

int plugin_is_GPL_compatible;

// (gsl_blas_dgemm M0 M1)
// Caveat: This only works for 2d vectors right now.
// Adapted from https://www.gnu.org/software/gsl/doc/html/blas.html#examples
static emacs_value Fgsl_blas_dgemm (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  // These will be 2d vectors
  emacs_value M0 = args[0];
  emacs_value M1 = args[1];

  // I need to get the number of rows and columns of each one.
  size_t rows1 = env->vec_size(env, M0);
  size_t cols1 = 0;
  
  // columns are trickier. We check the first element of the array. If it is a
  // vector, we get its length, otherwise it is a 1d vector and we should raise
  // an error. I don't know how to do that yet.
  emacs_value el1 = env->vec_get (env, M0, 0);
  if (env->eq(env,
	      env->type_of(env, el1),
	      env->intern(env, "vector")))
    {
      cols1 = env->vec_size(env, el1);
    } 
  
  // Now we know M0 has dimensions (rows1, cols1)

  size_t rows2 = env->vec_size(env, M1);
  size_t cols2 = 0;
  emacs_value el2 = env->vec_get (env, M1, 0);
  if (env->eq(env,
	      env->type_of(env, el2),
	      env->intern(env, "vector")))
    {
      cols2 = env->vec_size(env, el2);
    }

  // Now we know M1 had dimensions (rows2, cols2)
  
  // Now we have to build up arrays.
  // We are looking at a * b = c
  double a[rows1 * cols1], b[rows2 * cols2];
  double c[rows1 * cols2];

  //populate a
  emacs_value row, ij;
  for (int i = 0; i < rows1; i++)
    {
      row = env->vec_get(env, M0, i);
      for (int j = 0; j < cols1; j++)
  	{
  	  // get M0[i, j]
  	  ij = env->vec_get(env, row, j);
  	  a[cols1 * i + j] = extract_double(env, ij);
  	}
    }

  // populate b
  for (int i = 0; i < rows2; i++)
    {
      row = env->vec_get(env, M1, i);
      for (int j = 0; j < cols2; j++)
  	{	  
  	  // get M0[i, j]
  	  ij = env->vec_get(env, row, j);
  	  b[cols2 * i + j] = extract_double(env, ij);
  	}
    }

  // initialize c.  The solution will have dimensions of (rows1, cols2).
  for (int i = 0; i < rows1; i++)
    {
      for (int j = 0; j < cols2; j++)
  	{
  	  c[cols2 * i + j] = 0.0;
  	}
    }

  gsl_matrix_view A = gsl_matrix_view_array(a, rows1, cols1);
  gsl_matrix_view B = gsl_matrix_view_array(b, rows2, cols2);
  gsl_matrix_view C = gsl_matrix_view_array(c, rows1, cols2);

  /* Compute C = A B */
  gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
                  1.0, &A.matrix, &B.matrix,
                  0.0, &C.matrix);

  // printf ("[ %g, %g\n ]", b[0], b[1]);
  // printf ("[ %g, %g\n ]", b[2], b[3]);
  // printf ("[ %g, %g\n ]", b[4], b[5]);

  // Finally, C is a gsl_matrix_view_array. We need to build up a vector to
  // return. it has rows1 vectors of length cols2
  emacs_value *array = malloc(sizeof(emacs_value) * rows1);
  emacs_value *row1;
  emacs_value vector = env->intern(env, "vector");
  emacs_value vec;
  for (int i=0; i < rows1; i++)
    {
      row1 = malloc(sizeof(emacs_value) * cols2);
      for (int j=0; j < cols2; j++)
  	{
  	  row1[j] = env->make_float(env, gsl_matrix_get(&C.matrix, i, j));
  	}
      vec = env->funcall(env, vector, cols2, row1);
      array[i] = vec;
      free(row1);
    }
  emacs_value result = env->funcall(env, vector, rows1, array);
  free(array);
  return result;
}

static emacs_value Fgsl_linalg_LU_solve (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  // (solve A b) A and b are vectors  
  emacs_value A = args[0];
  emacs_value b = args[1];
  
  size_t n = env->vec_size(env, args[1]);
   
  double a_data[n][n];
  double b_data[n];

  emacs_value val;

  // copy data over to the arrays
  for (ptrdiff_t i = 0; i < n; i++)
    {
      val = env->vec_get(env, b, i);
      b_data[i] = extract_double(env, val);      
    }

  for (ptrdiff_t i = 0; i < n; i++)
    {
      emacs_value row = env->vec_get (env, A, i);
      for (ptrdiff_t j = 0; j < n; j++)
	{
	  val = env->vec_get(env, row, j);
	  a_data[i][j] = extract_double(env, val);
	}
    }
  
  gsl_matrix_view m = gsl_matrix_view_array (a_data, n, n);
  gsl_vector_view bb = gsl_vector_view_array (b_data, n);
  gsl_vector *x = gsl_vector_alloc (n);

  int s;

  gsl_permutation * p = gsl_permutation_alloc (n);
  gsl_linalg_LU_decomp (&m.matrix, p, &s);
  gsl_linalg_LU_solve (&m.matrix, p, &bb.vector, x);
 
  emacs_value *array = malloc(sizeof(emacs_value) * n);
  for (ptrdiff_t i = 0; i < n; i++)
    {
      array[i] = env->make_float(env, gsl_vector_get(x, i));      
    }
  
  emacs_value Fvector = env->intern(env, "vector");
  emacs_value vec = env->funcall(env, Fvector, n, array);
  free(array);
  return vec;
}

int emacs_module_init(struct emacs_runtime *ert)
{
  emacs_env *env = ert->get_environment(ert);
  
  DEFUN("gsl-linalg-LU-solve", Fgsl_linalg_LU_solve, 2, 2,
	"(gsl-linalg-LU-solve A b).\n" \
	"Solve A x = b for x.\n" \
	"Returns a vector containing the solution x.",
	NULL);


  DEFUN("gsl-blas-dgemm", Fgsl_blas_dgemm, 2, 2,
	"(gsl-blas-dgemm M0 M1)\n" \
	"Matrix multiply M0 and M1.\n" \
	"Both matrices must be 2D vectors.\n"\
	"Returns a vector.",
	NULL);
  provide(env, "gsl-linalg");
  
  return 0;
}
