//
// Created by mats on 2020-11-08.
//

#ifndef MATSLISP_SRC_IO_H_
#define MATSLISP_SRC_IO_H_

#include <stdio.h>

struct object;
struct workspace_t;

struct object* read(struct workspace_t* workspace, FILE *fp);
struct object* read_list(struct workspace_t* workspace, FILE *fp, int delimiter);
void write(FILE *fp, struct object *obj);

#endif //MATSLISP_SRC_IO_H_
