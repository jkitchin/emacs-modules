#include "../emacs-module.h"
#include "../emacs-module-helpers.h"
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
int plugin_is_GPL_compatible;

static emacs_value Fzmq_ctx_new (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  void *context = zmq_ctx_new ();
  return env->make_user_ptr(env,
			    NULL, // the finalizer func?
			    context);
}

// http://api.zeromq.org/4-2:zmq-ctx-destroy
// int zmq_ctx_destroy (void *context);
// (zmq-ctx-destroy context)
static emacs_value Fzmq_ctx_destroy (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  intptr_t context = env->get_user_ptr(env, args[0]);
  int result = zmq_ctx_destroy (context);
  return env->make_integer(env, result);
}


// http://api.zeromq.org/4-2:zmq-socket
// void *zmq_socket (void *context, int type);
//(zmq-socket context ZMQ_REQ)
static emacs_value Fzmq_socket (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{

  intptr_t context = env->get_user_ptr(env, args[0]);
  int type = env->extract_integer(env, args[1]);
  void *socket = zmq_socket (context, type);
  return env->make_user_ptr(env,
			    NULL, // the finalizer func?
			    socket);
}

// http://api.zeromq.org/4-2:zmq-close
// int zmq_close (void *socket);
// (zmq-close socket)
static emacs_value Fzmq_close (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  intptr_t socket = env->get_user_ptr(env, args[0]);
  int result = zmq_close (socket);
  return env->make_integer(env, result);
}


// http://api.zeromq.org/4-2:zmq-connect
// int zmq_connect (void *socket, const char *endpoint);
// (zmq-connect socket endpoint)
static emacs_value Fzmq_connect (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  intptr_t socket = env->get_user_ptr(env, args[0]);
  emacs_value endpoint = args[1];
  
  ptrdiff_t size = 0;
  // The first one will fail and write the length of the variable to size
  env->copy_string_contents(env, endpoint, NULL, &size);

  // Now we know the size and can do the copy
  char *endpoint_str = malloc(size);
  env->copy_string_contents(env, endpoint, endpoint_str, &size);

  int result = zmq_connect (socket, endpoint_str);
  free(endpoint_str);
  return env->make_integer(env, result);
}

// http://api.zeromq.org/4-2:zmq-send
// int zmq_send (void *socket, void *buf, size_t len, int flags);
// (zmq-send socket buf flag)
static emacs_value Fzmq_send (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  intptr_t socket = env->get_user_ptr(env, args[0]);

  emacs_value buf = args[1];
  
  ptrdiff_t size = 0;
  // The first one will fail and write the length of the variable to size
  env->copy_string_contents(env, buf, NULL, &size);

  // Now we know the size and can do the copy
  char *buf_str = malloc(size);
  env->copy_string_contents(env, buf, buf_str, &size);

  int flags = env->extract_integer(env, args[2]);

  int result= zmq_send(socket, buf_str, strlen(buf_str), flags);
  free(buf_str);
  return env->make_integer(env, result);
}

// http://api.zeromq.org/4-2:zmq-recv
// int zmq_recv (void *socket, void *buf, size_t len, int flags);
// (zmq-recv socket len flags)
static emacs_value Fzmq_recv (emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
  intptr_t socket = env->get_user_ptr(env, args[0]);
  int len = env->extract_integer(env, args[1]);
  int flags = env->extract_integer(env, args[2]);

  char cbuffer[len];

  int res = zmq_recv (socket, cbuffer, len, flags);

  emacs_value result = env->make_string(env, cbuffer, strlen(cbuffer) - 1);
  emacs_value ret = env->make_integer(env, res);

  emacs_value list = env->intern(env, "list");
  emacs_value largs[] = {result, ret};
  
  return env->funcall(env, list, 2, largs);
  
}


int emacs_module_init(struct emacs_runtime *ert)
{
  emacs_env *env = ert->get_environment(ert);


  DEFUN("zmq-ctx-new", Fzmq_ctx_new, 0, 0,
	"(zmq-ctx-new)\n" \
	"create new ØMQ context. Returns pointer to context.\n" \
	"http://api.zeromq.org/4-2:zmq-ctx-new",
	NULL);
  
  DEFUN("zmq-ctx-destroy", Fzmq_ctx_destroy, 1, 1,
	"(zmq-ctx-destroy CONTEXT)\n" \
	"terminate a ØMQ context\n" \
	"http://api.zeromq.org/4-2:zmq-ctx-destroy", NULL);

  // Defines a constant variable
  defconsti(env, "ZMQ-REQ", (int)ZMQ_REQ, "ZMQ_REQ");

  DEFUN("zmq-socket", Fzmq_socket, 2, 2,
	"(zmq-socket CONTEXT TYPE)\n"
	"create ØMQ socket. Returns pointer to socket."\
	"CONTEXT is from `zmq-ctx-new`.\n"
	"see http://api.zeromq.org/4-2:zmq-socket for TYPE info.", NULL);
  
  DEFUN("zmq-close", Fzmq_close, 1, 1,
	"(zmq-close SOCKET)\n" \       
	"close ØMQ socket. SOCKET is from `zmq-socket`\n" \
	"Returns status of call."\
	"http://api.zeromq.org/4-2:zmq-close",	
	NULL);

  DEFUN("zmq-connect", Fzmq_connect, 2, 2,
	"(zmq-connect SOCKET ENDPOINT)\n" \
	"Create outgoing connection from socket to endpoint." \
	"The endpoint is a string consisting of a transport :// followed by an address. The transport specifies the underlying protocol to use. The address specifies the transport-specific address to connect to." \
	"Returns result of call."\
	"http://api.zeromq.org/4-2:zmq-connect",
	NULL);
  
  DEFUN("zmq-send", Fzmq_send, 3, 3,
	"(zmq-send SOCKET BUF FLAGS)\n" \
	"send a message part on a socket" \
	"SOCKET is from `zmq-socket`." \
	"BUF is a string to send." \
	"FLAGS is an int." \
	"http://api.zeromq.org/4-2:zmq-send", NULL);

  DEFUN("zmq-recv", Fzmq_recv, 3, 3,
	"(zmq-recv SOCKET LEN FLAGS)\n" \
	"receive a message part from a socket.\n" \
	"SOCKET is from `zmq-socket`." \
	"LEN is the number of bytes to get." \
	"FLAGS is an int." \
	"Returns (result status)." \
	"http://api.zeromq.org/4-2:zmq-recv", NULL);


  provide(env, "mod-zmq");
  
  return 0;
}
