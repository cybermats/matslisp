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
typedef struct object *oop;

struct object {
  union {
    struct {
      oop head;
      oop tail;
    };
    struct {
      unsigned int type;
      union {
        symbol_t name;
        int integer;
        oop args;
      };
    };
  };
#ifndef NDEBUG
  int alive;
#endif
};

extern oop TEE;
extern oop nil;

struct workspace_t;

oop new_symbol(struct workspace_t *workspace, symbol_t name);
oop new_number(struct workspace_t *workspace, int integer);
oop new_cons(struct workspace_t *workspace, oop head, oop tail);
oop new_lambda(struct workspace_t *workspace, oop args, oop sexp);

int symbolp(oop obj);
int numberp(oop obj);
int consp(oop obj);
int lambdap(oop obj);

symbol_t get_name(oop obj);

void render_name(oop obj, char *buffer, size_t n);

oop eval(struct workspace_t *workspace, oop sexp, oop env);

void append(struct workspace_t *workspace, oop list, oop obj);



static inline int getType(oop obj) {
  if (obj) {
    if (obj->type > PAIR)
      return PAIR;
    else
      return (int)obj->type;
  }
  return UNDEFINED;
}

#define myis(TYPE, OBJ) ((OBJ) && (TYPE == getType(OBJ)))


#ifndef NDEBUG
#define checkType(OBJ, TYPE)  _checkType(OBJ, TYPE, #TYPE, __FILE__, __LINE__)
#ifdef __cplusplus
static inline oop _checkType(oop obj, int type, std::string name, std::string file, int line) {
#else
static inline oop _checkType(oop obj, int type, char *name, char *file, int line) {
#endif
  if (obj && !obj->alive)
    fatal("%s:%i: attempt to access dead object %s\n", file, line, name);
  if (!myis(ZERO, obj) && !myis(type, obj))
    fatal("%s:%i: typecheck failed for %s (%i != %i)\n", file, line, name, type, getType(obj));
  return obj;
}
#else
#define checkType(OBJ, TYPE) (OBJ)
#endif

#define myget(OBJ, TYPE, FIELD) (checkType(OBJ, TYPE)->FIELD)
#define myset(OBJ, TYPE, FIELD, VALUE) (checkType(OBJ, TYPE)->FIELD = (VALUE))

#define getHead(OBJ) (myget(OBJ, PAIR, head))
#define getTail(OBJ) (myget(OBJ, PAIR, tail))
#define getArg(OBJ) (myget(OBJ, LAMBDA, args))

#define setHead(OBJ, VAL) (myset(OBJ, PAIR, head, VAL))
#define setTail(OBJ, VAL) (myset(OBJ, PAIR, tail, VAL))
#define setArg(OBJ, VAL) (myset(OBJ, LAMBDA, args, VAL))

static inline oop car(oop obj) {
  return myis(PAIR, obj) ? getHead(obj) : nil;
}

static inline oop cdr(oop obj) {
  return myis(PAIR, obj) ? getTail(obj) : nil;
}
static inline oop  caar(oop  obj)		{ return car(car(obj)); }
static inline oop  cadr(oop  obj)		{ return car(cdr(obj)); }
static inline oop  cdar(oop  obj)		{ return cdr(car(obj)); }
static inline oop  cddr(oop  obj)		{ return cdr(cdr(obj)); }

#endif //MATSLISP_SRC_OBJECT_H_
