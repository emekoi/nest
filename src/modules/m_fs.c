/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdlib.h>
#include "util/util.h"
#include "util/fs.h"

#define MODULE_NAME "fs"
#define CLASS_NAME_FS "FS"


static void checkError(WrenVM *W, int32_t err, const char *str) {
  if (!err) return;
  if (err == FS_ENOWRITEPATH || !str) {
    wrenError(W, "%s", fs_errorStr(err));
  }
  wrenError(W, "%s '%s'", fs_errorStr(err), str);
}


static void w_fs_mount(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  int32_t res = fs_mount(path);
  checkError(W, res, path);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_unmount(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  int32_t res = fs_unmount(path);
  checkError(W, res, path);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_setWritePath(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  int32_t res = fs_setWritePath(path);
  checkError(W, res, path);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_exists(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  wrenSetSlotBool(W, 0, fs_exists(filename));
}


static void w_fs_getSize(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  size_t sz;
  int32_t res = fs_size(filename, &sz);
  checkError(W, res, filename);
  wrenSetSlotDouble(W, 0, sz);
}


static void w_fs_getModified(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  uint32_t t;
  int32_t res = fs_modified(filename, &t);
  checkError(W, res, filename);
  wrenSetSlotDouble(W, 0, t);
}


static void w_fs_read(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  size_t len;
  char *data = fs_read(filename, &len);
  if (!data) {
    wrenError(W, "could not read file '%s'", filename);
  }
  wrenSetSlotBytes(W, 0, data, len);
  free(data);
}


static void w_fs_isDir(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  wrenSetSlotBool(W, 0, fs_isDir(filename));
}


static void w_fs_listDir(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  fs_FileListNode *list = fs_listDir(path);
  wrenSetSlotNewList(W, 0);
  fs_FileListNode *n = list;
  for (int32_t i = 0; n; i++) {
    wrenSetSlotString(W, 1, n->name);
    wrenInsertInList(W, 0, i, 1);
    n = n->next;
  }
  fs_freeFileList(list);
}


static void w_fs_write(WrenVM *W) {
  wrenEnsureSlots(W, 3);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  wrenCheckSlot(W, 2, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  int32_t len;
  const char *data = wrenGetSlotBytes(W, 2, &len);
  int32_t res = fs_write(filename, data, len);
  checkError(W, res, filename);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_append(WrenVM *W) {
  wrenEnsureSlots(W, 3);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  wrenCheckSlot(W, 2, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  int32_t len;
  const char *data = wrenGetSlotBytes(W, 2, &len);
  int32_t res = fs_append(filename, data, len);
  checkError(W, res, filename);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_delete(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  int32_t res = fs_delete(filename);
  checkError(W, res, filename);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_makeDirs(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  int32_t res = fs_makeDirs(path);
  checkError(W, res, path);
  wrenSetSlotBool(W, 0, true);
}


void wren_open_fs(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = wrenGetMethodMap(W);

  map_set(methods, MODULE_NAME CLASS_NAME_FS "mount(_)s",        w_fs_mount);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "unmount(_)s",      w_fs_unmount);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "setWritePath(_)s", w_fs_setWritePath);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "exists(_)s",       w_fs_exists);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "getSize(_)s",      w_fs_getSize);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "getModified(_)s",  w_fs_getModified);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "read(_)s",         w_fs_read);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "isDir(_)s",        w_fs_isDir);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "listDir(_)s",      w_fs_listDir);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "write(_,_)s",      w_fs_write);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "append(_,_)s",     w_fs_append);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "delete(_)s",       w_fs_delete);
  map_set(methods, MODULE_NAME CLASS_NAME_FS "makeDirs(_)s",     w_fs_makeDirs);
  atexit(fs_deinit);
 }