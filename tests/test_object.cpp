//
// Created by mats on 2020-11-10.
//

#include "catch/catch.hpp"
extern "C" {
#include <memory.h>
#include <object.h>
}

#define WORKSPACE_SIZE 64

#define CREATE_WORKSPACE() \
  struct workspace_t *workspace = create_workspace(WORKSPACE_SIZE); \
  nil = obj_alloc(workspace);                                       \
  nil->head = nil->tail = nullptr;                                  \
  n_push(nil);

#define DELETE_WORKSPACE() \
  free_workspace(workspace);

TEST_CASE("new_symbol", "[object]") {
  CREATE_WORKSPACE();
  for (symbol_t name = 0; name < 16; ++name) {
    oop symbol = new_symbol(workspace, name);
    REQUIRE(symbol->type == SYMBOL);
    REQUIRE(symbol->name == name);
  }
  DELETE_WORKSPACE();
}

TEST_CASE("new_number", "[object]") {
  CREATE_WORKSPACE();
  for (int number = -10000; number < 10000; number += 1000) {
    oop symbol = new_number(workspace, number);
    REQUIRE(symbol->type == NUMBER);
    REQUIRE(symbol->integer == number);
  }
  DELETE_WORKSPACE();
}

TEST_CASE("new_cons_null", "[object]") {
  CREATE_WORKSPACE();
  oop nil_pair = new_cons(workspace, nullptr, nullptr);
  n_push(nil_pair);
  REQUIRE(nil_pair->type == ZERO);
  REQUIRE_FALSE(nil_pair->head);
  REQUIRE_FALSE(nil_pair->tail);
  DELETE_WORKSPACE();
}

TEST_CASE("new_cons_nil", "[object]") {
  CREATE_WORKSPACE();
  oop nil_pair = new_cons(workspace, nil, nil);
  n_push(nil_pair);
  REQUIRE(nil_pair->type >= PAIR);
  REQUIRE(nil_pair->head == nil);
  REQUIRE(nil_pair->tail == nil);
  DELETE_WORKSPACE();
}

TEST_CASE("new_cons_pair", "[object]") {
  CREATE_WORKSPACE();

  oop head = obj_alloc(workspace);
  n_push(head);
  oop tail = obj_alloc(workspace);
  n_push(tail);

  oop pair = new_cons(workspace, head, tail);
  n_push(pair);
  REQUIRE(pair->type >= PAIR);
  REQUIRE(pair->head == head);
  REQUIRE(pair->tail == tail);
  DELETE_WORKSPACE();
}

TEST_CASE("new_lambda_nil", "[object]") {
  CREATE_WORKSPACE();
  oop lmd = new_lambda(workspace, nil, nil);
  REQUIRE(lmd->type == LAMBDA);
  REQUIRE(lmd->args->head == nil);
  REQUIRE(lmd->args->tail == nil);
  DELETE_WORKSPACE();
}

TEST_CASE("new_lambda_arg", "[object]") {
  CREATE_WORKSPACE();
  oop args = new_cons(workspace, nullptr, nullptr);
  n_push(args);
  oop sexp = new_cons(workspace, nullptr, nullptr);
  n_push(sexp);

  oop lmd = new_lambda(workspace, args, sexp);
  REQUIRE(lmd->type == LAMBDA);
  REQUIRE(lmd->args->head == args);
  REQUIRE(lmd->args->tail == sexp);
  DELETE_WORKSPACE();
}

TEST_CASE("symbolp", "[object]") {
  CREATE_WORKSPACE();
  oop obj = nil;
  REQUIRE_FALSE(symbolp(obj));
  obj = new_symbol(workspace, 0);
  REQUIRE(symbolp(obj));
  obj = new_number(workspace, 0);
  REQUIRE_FALSE(symbolp(obj));
  obj = new_cons(workspace, nil, nil);
  REQUIRE_FALSE(symbolp(obj));
  obj = new_lambda(workspace, nil, nil);
  REQUIRE_FALSE(symbolp(obj));
  DELETE_WORKSPACE();
}

TEST_CASE("numberp", "[object]") {
  CREATE_WORKSPACE();
  oop obj = nil;
  REQUIRE_FALSE(numberp(obj));
  obj = new_symbol(workspace, 0);
  REQUIRE_FALSE(numberp(obj));
  obj = new_number(workspace, 0);
  REQUIRE(numberp(obj));
  obj = new_cons(workspace, nil, nil);
  REQUIRE_FALSE(numberp(obj));
  obj = new_lambda(workspace, nil, nil);
  REQUIRE_FALSE(numberp(obj));
  DELETE_WORKSPACE();
}


TEST_CASE("consp", "[object]") {
  CREATE_WORKSPACE();
  oop obj = nil;
  REQUIRE_FALSE(consp(obj));
  obj = new_symbol(workspace, 0);
  REQUIRE_FALSE(consp(obj));
  obj = new_number(workspace, 0);
  REQUIRE_FALSE(consp(obj));
  obj = new_cons(workspace, nil, nil);
  REQUIRE(consp(obj));
  obj = new_lambda(workspace, nil, nil);
  REQUIRE_FALSE(consp(obj));
  DELETE_WORKSPACE();
}


TEST_CASE("lambdap", "[object]") {
  CREATE_WORKSPACE();
  oop obj = nil;
  REQUIRE_FALSE(lambdap(obj));
  obj = new_symbol(workspace, 0);
  REQUIRE_FALSE(lambdap(obj));
  obj = new_number(workspace, 0);
  REQUIRE_FALSE(lambdap(obj));
  obj = new_cons(workspace, nil, nil);
  REQUIRE_FALSE(lambdap(obj));
  obj = new_lambda(workspace, nil, nil);
  REQUIRE(lambdap(obj));
  DELETE_WORKSPACE();
}