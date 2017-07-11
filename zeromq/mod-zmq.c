#include "../emacs-module.h"
#include "../emacs-module-helpers.h"
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
int plugin_is_GPL_compatible;

// (zmq-send-and-recv msg) -> returns what is received
static emacs_value zmq_send_and_recv (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  emacs_value msg = args[0];

  // first we copy our string out of emacs and into a char array
  ptrdiff_t size = 0;
  // The first one will fail and write the length of the variable to size
  env->copy_string_contents(env, msg, NULL, &size);

  // Now we know the size and can do the copy
  char *str = malloc(size);
  env->copy_string_contents(env, msg, str, &size);

  // Now we connect and send.
  void *context = zmq_ctx_new ();
  void *requester = zmq_socket (context, ZMQ_REQ);

  // TODO: we need a user pointer maybe to create this.
  zmq_connect (requester, "tcp://localhost:5555");
  
  zmq_send (requester, str, strlen(str), 0);
  free(str);

  // TODO: need a way to know how big we will receive.
  char cbuffer [10];
  zmq_recv (requester, cbuffer, 20, 0);

  printf("Got %s back (%d chars).\n", cbuffer, strlen(cbuffer));

  wchar_t *hello_world = (char*)malloc(strlen(cbuffer) * sizeof(wchar_t*));
// Prints "Hello world!" on hello_world
  sprintf(hello_world, "%s", cbuffer);

  printf("hw contains %s (%d chars, %d wchars)\n", hello_world, strlen(hello_world), wcslen(hello_world));
  
  emacs_value result = env->make_string(env, hello_world, strlen(hello_world));
  zmq_close (requester);
  zmq_ctx_destroy (context);

  return result;
}

// This is a test to get a string back. It works fine.
static emacs_value stest (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  emacs_value msg = args[0];
  ptrdiff_t size = 0;
  // The first one will fail and write the length of the variable to size
  env->copy_string_contents(env, msg, NULL, &size);

  // Now we know the size and can do the copy
  char *str = malloc(size);
  env->copy_string_contents(env, msg, str, &size);

  char *hello_world = (char*)malloc(12 * sizeof(char));
// Prints "Hello world!" on hello_world
  sprintf(hello_world, "Hello %s!", str);
  
  emacs_value result = env->make_string(env, hello_world, strlen(hello_world));
  
  return result;

}

int emacs_module_init(struct emacs_runtime *ert)
{
  emacs_env *env = ert->get_environment(ert);

  DEFUN("zmq-send-and-recv", zmq_send_and_recv, 1, 1,
	"(zmq-send-and-recv arg)\n" \
	"Send ARG to server. Returns what server sends back.",
	NULL);

  DEFUN("stest", stest, 1, 1, NULL, NULL);


  provide(env, "mod-zmq");
  
  return 0;
}
