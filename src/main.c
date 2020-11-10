#include <stdio.h>

#include "object.h"
#include "memory.h"
#include "io.h"
#include "strings.h"
#include "error_handling.h"

#define WORKSPACESIZE 1024*1024

const char *t_str = "t\0\0\0";

oop initialize_environment(struct workspace_t *workspace) {
  TEE = new_symbol(workspace, pack40(t_str, 4));
  push_root(workspace, TEE);
  nil = new_cons(workspace, NULL, NULL);
  push_root(workspace, nil);
  return new_cons(workspace, NULL, NULL);
}

oop reinitialize_environment(struct workspace_t *workspace) {
  workspace->root = NULL;
  n_push(workspace->env);
  TEE = new_symbol(workspace, pack40(t_str, 4));
  n_push(TEE);
  return workspace->env;
}


int main() {
  printf("Mats Lisp v0.1\n");
  struct workspace_t* workspace = create_workspace(WORKSPACESIZE);
  if (!setjmp(exception)) {
    workspace->env = initialize_environment(workspace);
  } else {
    reinitialize_environment(workspace);
  }
  push_root(workspace, workspace->env);
  for(;;) {
#ifndef NDEBUG
    printf("Mem (%d / %d) ", workspace->total_space - workspace->free_space, workspace->total_space);
#endif
    gc(workspace);
    fflush(stderr);
    fflush(stdout);
    printf("> ");
    fflush(stdout);
    oop sexp = read(workspace, stdin);
    if (sexp == (oop)EOF)
      break;
    push_root(workspace, sexp);

#ifndef NDEBUG
    printf("Read: ");
    write(stdout, sexp);
    printf("\n");
#endif
    sexp = eval(workspace, sexp, workspace->env);
#if 0 //ifndef NDEBUG
    printf("Env: ");
    write(stdout, workspace->env);
    printf("\nEval: ");
#endif
    write(stdout, sexp);
    printf("\n");

    pop_root(workspace);
//    gc(workspace);
  }
  free_workspace(workspace);
  printf("Exiting...\n");
}
