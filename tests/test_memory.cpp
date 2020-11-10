//
// Created by mats on 2020-11-10.
//

#include "catch/catch.hpp"
extern "C" {
#include <memory.h>
#include <object.h>
}

#define WORKSPACE_SIZE 64

#ifndef NDEBUG
#define CHECK_ALIVE(obj) REQUIRE(obj->alive)
#define CHECK_DEAD(obj) REQUIRE_FALSE(obj->alive)
#else
#define CHECK_ALIVE(obj) REQUIRE(obj)
#define CHECK_DEAD(obj) REQUIRE(obj)
#endif

TEST_CASE("set up workspace", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  REQUIRE(workspace->total_space == WORKSPACE_SIZE);
  REQUIRE(workspace->pool != NULL);
  free_workspace(workspace);
}

TEST_CASE("allocate object", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  oop obj = obj_alloc(workspace);
  REQUIRE(obj);
  CHECK_ALIVE(obj);
  obj_free(workspace, obj);
  CHECK_DEAD(obj);
  REQUIRE_FALSE(obj->head);
  free_workspace(workspace);
}

TEST_CASE("push_root_basic", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  REQUIRE_FALSE(workspace->root);
  oop obj = obj_alloc(workspace);
  push_root(workspace, obj);
  REQUIRE(workspace->root);
  REQUIRE(workspace->root->head == obj);
  pop_root(workspace);
  REQUIRE_FALSE(workspace->root);
  free_workspace(workspace);
}

TEST_CASE("gc", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE);
  oop obj = obj_alloc(workspace);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-1);
  REQUIRE(gc(workspace) == 1);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE);
  CHECK_DEAD(obj);
}

TEST_CASE("push_root_gc", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  oop obj = obj_alloc(workspace);
  push_root(workspace, obj);
  REQUIRE_FALSE(gc(workspace));
  CHECK_ALIVE(obj);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 2);
  CHECK_DEAD(obj);
}


TEST_CASE("gc_links_SYMBOL", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  oop obj = obj_alloc(workspace);
  obj->type = SYMBOL;
  push_root(workspace, obj);
  REQUIRE(gc(workspace) == 0);
  CHECK_ALIVE(obj);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 2);
  CHECK_DEAD(obj);
}

TEST_CASE("gc_links_NUMBER", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  oop obj = obj_alloc(workspace);
  obj->type = NUMBER;
  push_root(workspace, obj);
  REQUIRE(gc(workspace) == 0);
  CHECK_ALIVE(obj);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 2);
  CHECK_DEAD(obj);
}

TEST_CASE("gc_links_LAMBDA", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  oop obj = obj_alloc(workspace);
  obj->type = LAMBDA;
  obj->args = nullptr;
  push_root(workspace, obj);
  oop arg = obj_alloc(workspace);
  arg->head = nullptr;
  arg->tail = nullptr;
  obj->args = arg;
  CHECK_ALIVE(obj);
  CHECK_ALIVE(arg);
  REQUIRE(gc(workspace) == 0);
  CHECK_ALIVE(obj);
  CHECK_ALIVE(arg);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 3);
  CHECK_DEAD(obj);
  CHECK_DEAD(arg);
}

TEST_CASE("gc_links_PAIR", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  oop obj = obj_alloc(workspace);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-1);
  obj->type = PAIR;
  obj->head = nullptr;
  obj->tail = nullptr;
  push_root(workspace, obj);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-2);
  oop head = obj_alloc(workspace);
  head->head = nullptr;
  head->tail = nullptr;
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-3);
  obj->head = head;
  CHECK_ALIVE(obj);
  CHECK_ALIVE(head);
  oop tail = obj_alloc(workspace);
  tail->head = nullptr;
  tail->tail = nullptr;
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-4);
  obj->tail = tail;
  CHECK_ALIVE(obj);
  CHECK_ALIVE(head);
  CHECK_ALIVE(tail);
  REQUIRE(gc(workspace) == 0);
  CHECK_ALIVE(obj);
  CHECK_ALIVE(head);
  CHECK_ALIVE(tail);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 4);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE);
  CHECK_DEAD(obj);
  CHECK_DEAD(head);
  CHECK_DEAD(tail);
}

