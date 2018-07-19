/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#if _WIN32
  #include <windows.h>
#elif __linux__
  #include <unistd.h>
#elif __APPLE__
  #include <mach-o/dyld.h>
  #include "TargetConditionals.h"
#endif

#include "util/util.h"

#define MODULE_NAME "os"
#define CLASS_NAME_PLATFORM "Platform"
#define CLASS_NAME_PROCESS "Process"

int32_t numArgs;
const char** args;

void w_os_setArguments(int32_t argc, const char *argv[]) {
  numArgs = argc;
  args = argv;
}


void w_platform_name(WrenVM* W) {
  wrenEnsureSlots(W, 1);

  #ifdef _WIN32
    wrenSetSlotString(W, 0, "windows");
  #elif __APPLE__
    #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
      wrenSetSlotString(W, 0, "ios");
    #elif TARGET_OS_MAC
      wrenSetSlotString(W, 0, "osx");
    #else
      wrenSetSlotString(W, 0, "unknown");
    #endif
  #elif __linux__
    wrenSetSlotString(W, 0, "linux");
  #elif __unix__
    wrenSetSlotString(W, 0, "unix");
  #elif defined(_POSIX_VERSION)
    wrenSetSlotString(W, 0, "posix");
  #else
    wrenSetSlotString(W, 0, "unknown");
  #endif
}


void w_platform_isPosix(WrenVM* W) {
  wrenEnsureSlots(W, 1);
  
  #ifdef _WIN32
    wrenSetSlotBool(W, 0, false);
  #elif __APPLE__
    wrenSetSlotBool(W, 0, true);
  #elif __linux__
    wrenSetSlotBool(W, 0, true);
  #elif __unix__
    wrenSetSlotBool(W, 0, true);
  #elif defined(_POSIX_VERSION)
    wrenSetSlotBool(W, 0, true);
  #else
    wrenSetSlotString(W, 0, false);
  #endif
}


static char *dirname(char *str) {
  char *p = str + strlen(str);
  while (p != str) {
    if (*p == '/' || *p == '\\') {
      *p = '\0';
      break;
    }
    p--;
  }
  return str;
}


void w_platform_exeDir(WrenVM* W) {
  wrenEnsureSlots(W, 1);
  
#if _WIN32
  char buf[1024];
  int32_t len = GetModuleFileName(NULL, buf, sizeof(buf) - 1);
  buf[len] = '\0';
  dirname(buf);
  wrenSetSlotStringFormat(W, 0, "%s", buf);
#elif __linux__
  uint8_t path[128];
  uint8_t buf[1024];
  sprintf(path, "/proc/%d/exe", getpid());
  int32_t len = readlink(path, buf, sizeof(buf) - 1);
  ASSERT( len != -1 );
  buf[len] = '\0';
  dirname(buf);
  wrenSetSlotStringFormat(W, 0, "%s", buf);
#elif __FreeBSD__
  /* TODO : Implement this */
  wrenSetSlotString(W, 0, ".");
#elif __APPLE__
  uint8_t buf[1024];
  uint32_t size = sizeof(buf);
  ASSERT( _NSGetExecutablePath(buf, &size) == 0 );
  dirname(buf);
  wrenSetSlotStringFormat(W, 0, "%s", buf);
#else
  wrenSetSlotString(W, 0, ".");
#endif
}


void w_platform_appData(WrenVM* W) {
  wrenEnsureSlots(W, 1);
  
#if _WIN32
  wrenSetSlotStringFormat(W, 0, "%s", getenv("APPDATA"));
#elif __APPLE__
  wrenSetSlotStringFormat(W, 0, "%s/Library/Application Support", getenv("HOME"));
#else
  wrenSetSlotStringFormat(W, 0, "%s/.local/share", getenv("HOME"));
#endif
}


void w_process_allArguments(WrenVM* W) {
  wrenEnsureSlots(W, 2);
  wrenSetSlotNewList(W, 0);

  for (int32_t i = 0; i < numArgs; i++) {
    wrenSetSlotString(W, 1, args[i]);
    wrenInsertInList(W, 0, -1, 1);
  }
}


void wren_open_os(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = wrenGetMethodMap(W);

  map_set(methods, MODULE_NAME CLASS_NAME_PLATFORM "names",       w_platform_name);
  map_set(methods, MODULE_NAME CLASS_NAME_PLATFORM "isPosixs",    w_platform_isPosix);
  map_set(methods, MODULE_NAME CLASS_NAME_PLATFORM "exeDirs",     w_platform_exeDir);
  map_set(methods, MODULE_NAME CLASS_NAME_PLATFORM "appDatas",    w_platform_appData);

  map_set(methods, MODULE_NAME CLASS_NAME_PROCESS "allArgumentss", w_process_allArguments);
 }