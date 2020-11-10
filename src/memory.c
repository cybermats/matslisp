//
// Created by mats on 2020-11-08.
//

#include <stdint.h>
#include <stdio.h>
#include "memory.h"
#include "object.h"
#include "error_handling.h"

struct workspace_t* create_workspace(int workspace_size) {
  struct workspace_t* workspace = (struct workspace_t*)malloc(sizeof(struct workspace_t));
  if (!workspace)
    return NULL;

  size_t pool_size = sizeof(struct object);
  workspace->pool = (struct object*) malloc(workspace_size * pool_size);
  if (!workspace->pool) {
    free(workspace);
    return NULL;
  }
  workspace->free_space = 0;
  workspace->free_list = NULL;
  workspace->total_space = workspace_size;
  workspace->env = NULL;
  workspace->root = NULL;

  for (int i = workspace_size - 1; i >= 0; --i) {
    struct object* obj = &workspace->pool[i];
    obj->head = NULL;
    obj->tail = workspace->free_list;
    workspace->free_list = obj;
    workspace->free_space++;
#ifndef NDEBUG
    obj->alive = 0;
#endif
  }
  return workspace;
}

void free_workspace(struct workspace_t *workspace) {
  if (workspace) {
    if (workspace->pool) {
      free(workspace->pool);
    }
    free(workspace);
  }
}

struct object* obj_alloc(struct workspace_t* workspace) {
  gc(workspace);
  if (workspace->free_space == 0)
    fatal("no room");

  struct object* tmp = workspace->free_list;
  workspace->free_list = workspace->free_list->tail;
  workspace->free_space--;
#ifndef NDEBUG
  tmp->alive = 1;
#endif
  return tmp;
}

struct object* obj_alloc_without_gc(struct workspace_t* workspace) {
  if (workspace->free_space == 0)
    fatal("no room");
  struct object* tmp = workspace->free_list;
  workspace->free_list = workspace->free_list->tail;
  workspace->free_space--;
#ifndef NDEBUG
  tmp->alive = 1;
#endif
  return tmp;
}


void obj_free(struct workspace_t* workspace, struct object* obj) {
  obj->head = NULL;
  obj->tail = workspace->free_list;
  workspace->free_list = obj;
  workspace->free_space++;
#ifndef NDEBUG
  obj->alive = 0;
#endif
}

void push_root(struct workspace_t *workspace, struct object* obj) {
  struct object *cons = obj_alloc_without_gc(workspace);
  cons->head = obj;
  cons->tail= workspace->root;
  workspace->root = cons;
  gc(workspace);
}
void pop_root(struct workspace_t *workspace) {
  workspace->root = workspace->root->tail;
}

#define MARKBIT 1

void mark(struct object *obj) {
  obj->head = (struct object*)(((uintptr_t)obj->head) | MARKBIT);
}

void unmark(struct object *obj) {
  obj->head = (struct object*)(((uintptr_t)obj->head) & ~MARKBIT);
}

int marked(struct object *obj) {
  return ((((uintptr_t)(obj->head)) & MARKBIT) != 0);
}

void markobject(struct object *obj) {
  while(1) {
    if (obj == NULL)
      return;
    if (marked(obj))
      return;

    struct object *arg = obj->head;
    unsigned int type = obj->type;
    struct object *lambda = obj->args;
    mark(obj);

    if (type >= PAIR || type == ZERO) {
      markobject(arg);
      obj = obj->tail;
    } else if (type == LAMBDA) {
      markobject(lambda);
      break;
    }
    else {
      break;
    }

  }
}
void sweep(struct workspace_t *workspace) {
  workspace->free_space = 0;
  workspace->free_list = NULL;
  for (int i = workspace->total_space - 1; i>=0; --i) {
    struct object *obj = &workspace->pool[i];
    if (!marked(obj))
      obj_free(workspace, obj);
    else
      unmark(obj);
  }
}

int gc(struct workspace_t *workspace) {
#ifndef NDEBUG
  return gc_force(workspace);
#else
  if (workspace->free_space <= 5) {
    return gc_force(workspace);
  }
#endif
return 0;
}

int gc_force(struct workspace_t *workspace) {
  int start = workspace->free_space;

  if (workspace->root) {
    markobject(workspace->root);
  }
  sweep(workspace);
  int freed = workspace->free_space - start;
#ifndef NDEBUG
  fprintf(stderr,"GC: %d freed.\n",  freed);
#endif
  return freed;
}