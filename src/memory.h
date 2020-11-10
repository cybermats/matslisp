//
// Created by mats on 2020-11-08.
//

#ifndef MATSLISP_SRC_MEMORY_H_
#define MATSLISP_SRC_MEMORY_H_

#include <stdlib.h>

struct object;
typedef struct object *oop;

struct workspace_t {
  oop pool;
  oop free_list;
  int free_space;
  oop root;
  int total_space;
  oop env;
};

#define n_push(OBJ) push_root(workspace, (OBJ));
#define n_pop() pop_root(workspace);

struct workspace_t* create_workspace(int workspace_size);
void free_workspace(struct workspace_t *workspace);


oop obj_alloc(struct workspace_t* workspace);
oop obj_alloc_without_gc(struct workspace_t* workspace);
void obj_free(struct workspace_t* workspace, oop obj);

void push_root(struct workspace_t *workspace, oop obj);
void pop_root(struct workspace_t *workspace);

void markobject(oop obj);
void sweep(struct workspace_t *workspace);

int gc(struct workspace_t *workspace);
int gc_force(struct workspace_t *workspace);

#endif //MATSLISP_SRC_MEMORY_H_
