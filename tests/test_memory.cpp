//
// Created by mats on 2020-11-10.
//

#include "catch/catch.hpp"
extern "C" {
#include <memory.h>
#include <object.h>
}

#define WORKSPACE_SIZE 64

TEST_CASE("set up workspace", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  REQUIRE(workspace->total_space == WORKSPACE_SIZE);
  REQUIRE(workspace->pool != NULL);
  free_workspace(workspace);
}

TEST_CASE("allocate object", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  struct object *obj = obj_alloc(workspace);
  REQUIRE(obj);
  REQUIRE(obj->alive);
  obj_free(workspace, obj);
  REQUIRE_FALSE(obj->alive);
  REQUIRE_FALSE(obj->head);
  free_workspace(workspace);
}

TEST_CASE("push_root_basic", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  REQUIRE_FALSE(workspace->root);
  struct object *obj = obj_alloc(workspace);
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
  struct object *obj = obj_alloc(workspace);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-1);
  REQUIRE(gc(workspace) == 1);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE);
  REQUIRE_FALSE(obj->alive);
}

TEST_CASE("push_root_gc", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  struct object *obj = obj_alloc(workspace);
  push_root(workspace, obj);
  REQUIRE_FALSE(gc(workspace));
  REQUIRE(obj->alive);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 2);
  REQUIRE_FALSE(obj->alive);
}


TEST_CASE("gc_links_SYMBOL", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  struct object *obj = obj_alloc(workspace);
  obj->type = SYMBOL;
  push_root(workspace, obj);
  REQUIRE(gc(workspace) == 0);
  REQUIRE(obj->alive);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 2);
  REQUIRE_FALSE(obj->alive);
}

TEST_CASE("gc_links_NUMBER", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  struct object *obj = obj_alloc(workspace);
  obj->type = NUMBER;
  push_root(workspace, obj);
  REQUIRE(gc(workspace) == 0);
  REQUIRE(obj->alive);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 2);
  REQUIRE_FALSE(obj->alive);
}

TEST_CASE("gc_links_LAMBDA", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  struct object *obj = obj_alloc(workspace);
  obj->type = LAMBDA;
  obj->args = NULL;
  push_root(workspace, obj);
  struct object *arg = obj_alloc(workspace);
  arg->head = NULL;
  arg->tail = NULL;
  obj->args = arg;
  REQUIRE(obj->alive);
  REQUIRE(arg->alive);
  REQUIRE(gc(workspace) == 0);
  REQUIRE(obj->alive);
  REQUIRE(arg->alive);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 3);
  REQUIRE_FALSE(obj->alive);
  REQUIRE_FALSE(arg->alive);
}

TEST_CASE("gc_links_PAIR", "[workspace]") {
  struct workspace_t* workspace = create_workspace(WORKSPACE_SIZE);
  struct object *obj = obj_alloc(workspace);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-1);
  obj->type = PAIR;
  obj->head = NULL;
  obj->tail = NULL;
  push_root(workspace, obj);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-2);
  struct object *head = obj_alloc(workspace);
  head->head = NULL;
  head->tail = NULL;
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-3);
  obj->head = head;
  REQUIRE(obj->alive);
  REQUIRE(head->alive);
  struct object *tail = obj_alloc(workspace);
  tail->head = NULL;
  tail->tail = NULL;
  REQUIRE(workspace->free_space == WORKSPACE_SIZE-4);
  obj->tail = tail;
  REQUIRE(obj->alive);
  REQUIRE(head->alive);
  REQUIRE(tail->alive);
  REQUIRE(gc(workspace) == 0);
  REQUIRE(obj->alive);
  REQUIRE(head->alive);
  REQUIRE(tail->alive);
  pop_root(workspace);
  REQUIRE(gc(workspace) == 4);
  REQUIRE(workspace->free_space == WORKSPACE_SIZE);
  REQUIRE_FALSE(obj->alive);
  REQUIRE_FALSE(head->alive);
  REQUIRE_FALSE(tail->alive);
}

