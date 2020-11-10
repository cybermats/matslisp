//
// Created by mats on 2020-11-08.
//

#ifndef MATSLISP_SRC_MEMORY_H_
#define MATSLISP_SRC_MEMORY_H_

#include <stdlib.h>

struct object;

struct workspace_t {
  struct object *pool;
  struct object *free_list;
  int free_space;
  struct object *root;
  int total_space;
  struct object *env;
};

struct workspace_t* create_workspace(int workspace_size);
void free_workspace(struct workspace_t *workspace);


struct object* obj_alloc(struct workspace_t* workspace);
struct object* obj_alloc_without_gc(struct workspace_t* workspace);
void obj_free(struct workspace_t* workspace, struct object* obj);

void push_root(struct workspace_t *workspace, struct object* obj);
void pop_root(struct workspace_t *workspace);

void markobject(struct object *obj);
void sweep(struct workspace_t *workspace);

int gc(struct workspace_t *workspace);
int gc_force(struct workspace_t *workspace);

#endif //MATSLISP_SRC_MEMORY_H_
