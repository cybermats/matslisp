//
// Created by mats on 2020-11-08.
//

#ifndef MATSLISP_SRC_OBJECT_H_
#define MATSLISP_SRC_OBJECT_H_

#include <stddef.h>
#include "error_handling.h"

typedef unsigned int symbol_t;

enum type_t {
  ZERO = 0,
  SYMBOL = 2,
  NUMBER = 4,
  LAMBDA = 6,
  PAIR = 8,
  UNDEFINED,
};

struct object;

struct object {
  union {
    struct {
      struct object* head;
      struct object* tail;
    };
    struct {
      unsigned int type;
      union {
        symbol_t name;
        int integer;
        struct object* args;
      };
    };
  };
#ifndef NDEBUG
  int alive;
#endif
};

extern struct object *TEE;
extern struct object *nil;

struct workspace_t;

struct object* new_symbol(struct workspace_t *workspace, symbol_t name);
struct object* new_number(struct workspace_t *workspace, int integer);
struct object* new_cons(struct workspace_t *workspace, struct object *head, struct object *tail);
struct object *new_lambda(struct workspace_t *workspace, struct object *args, struct object *sexp);

int symbolp(struct object *obj);
int numberp(struct object *obj);
int consp(struct object *obj);
int lambdap(struct object *obj);

symbol_t get_name(struct object *obj);

void render_name(struct object *obj, char *buffer, size_t n);

struct object *eval(struct workspace_t *workspace, struct object *sexp, struct object *env);

void append(struct workspace_t *workspace, struct object *list, struct object *obj);



static inline int getType(struct object *obj) {
  if (obj) {
    if (obj->type > PAIR)
      return PAIR;
    else
      return obj->type;
  }
  return UNDEFINED;
}

#define is(TYPE, OBJ) ((OBJ) && (TYPE == getType(OBJ)))


#ifndef NDEBUG
#define checkType(OBJ, TYPE)  _checkType(OBJ, TYPE, #TYPE, __FILE__, __LINE__)
static inline struct object * _checkType(struct object * obj, int type, char *name, char *file, int line)
{

  if (obj && !obj->alive)
    fatal("%s:%i: attempt to access dead object %s\n", file, line, name);
  if (!is(ZERO, obj) && !is(type, obj))
    fatal("%s:%i: typecheck failed for %s (%i != %i)\n", file, line, name, type, getType(obj));
  return obj;
}
#else
#define checkType(OBJ, TYPE) (OBJ)
#endif

#define get(OBJ, TYPE, FIELD) (checkType(OBJ, TYPE)->FIELD)
#define set(OBJ, TYPE, FIELD, VALUE) (checkType(OBJ, TYPE)->FIELD = (VALUE))

#define getHead(OBJ) (get(OBJ, PAIR, head))
#define getTail(OBJ) (get(OBJ, PAIR, tail))
#define getArg(OBJ) (get(OBJ, LAMBDA, args))

#define setHead(OBJ, VAL) (set(OBJ, PAIR, head, VAL))
#define setTail(OBJ, VAL) (set(OBJ, PAIR, tail, VAL))
#define setArg(OBJ, VAL) (set(OBJ, LAMBDA, args, VAL))

static inline struct object *car(struct object *obj) {
  return is(PAIR, obj) ? getHead(obj) : nil;
}

static inline struct object *cdr(struct object *obj) {
  return is(PAIR, obj) ? getTail(obj) : nil;
}
static inline struct object * caar(struct object * obj)		{ return car(car(obj)); }
static inline struct object * cadr(struct object * obj)		{ return car(cdr(obj)); }
static inline struct object * cdar(struct object * obj)		{ return cdr(car(obj)); }
static inline struct object * cddr(struct object * obj)		{ return cdr(cdr(obj)); }

#endif //MATSLISP_SRC_OBJECT_H_
