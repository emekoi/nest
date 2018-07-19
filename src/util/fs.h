/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct fs_FileListNode {
  char *name;
  struct fs_FileListNode *next;
} fs_FileListNode;

enum {
  FS_ESUCCESS     = 0,
  FS_EFAILURE     = -1,
  FS_EOUTOFMEM    = -2,
  FS_EBADPATH     = -3,
  FS_EBADFILENAME = -4,
  FS_ENOWRITEPATH = -5,
  FS_ECANTOPEN    = -6,
  FS_ECANTREAD    = -7,
  FS_ECANTWRITE   = -8,
  FS_ECANTDELETE  = -9,
  FS_ECANTMKDIR   = -10,
  FS_ENOTEXIST    = -11,
};

const char *fs_errorStr(int32_t err);
void fs_deinit(void);
int32_t fs_mount(const char *path);
int32_t fs_unmount(const char *path);
int32_t fs_setWritePath(const char *path);
bool fs_exists(const char *filename);
int32_t fs_modified(const char *filename, uint32_t *mtime);
int32_t fs_size(const char *filename, size_t *size);
void *fs_read(const char *filename, size_t *size);
bool fs_isDir(const char *filename);
fs_FileListNode *fs_listDir(const char *path);
void fs_freeFileList(fs_FileListNode *list);
int32_t fs_write(const char *filename, const void *data, int32_t size);
int32_t fs_append(const char *filename, const void *data, int32_t size);
int32_t fs_delete(const char *filename);
int32_t fs_makeDirs(const char *path);
