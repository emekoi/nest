/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#pragma once

#include <stdio.h>
#include "wren.h"
#include "util.h"
#include "fs.h"


static char *concat(const char *str, ...) {
  va_list args;
  const char *s;
  /* Get len */
  int32_t len = strlen(str);
  va_start(args, str);
  while ((s = va_arg(args, char*))) {
    len += strlen(s);
  }
  va_end(args);
  /* Build string */
  char *res = malloc(len + 1);
  if (!res) return NULL;
  strcpy(res, str);
  va_start(args, str);
  while ((s = va_arg(args, char*))) {
    strcat(res, s);
  }
  va_end(args);
  return res;
}


static void wren_writeFn(WrenVM *W, const char *text) {
  UNUSED(W);
	printf("%s", text);
}


// static void wren_errorFn_original(
//   WrenVM* W, WrenErrorType type, const char* module,
//   int32_t line, const char* message
// ) {
//   UNUSED(W);
//   switch (type) {
//     case WREN_ERROR_COMPILE: {
//       fprintf(
//         stderr, "compile error:\n  %s: %d: %s\n",
//         module, line, message);
//       break;
//     }

//     case WREN_ERROR_RUNTIME: {
//       fprintf(stderr, "runtime error:\n  %s :%d: %s\ntraceback:\n", module, line, message);
//       break;
//     }

//     case WREN_ERROR_STACK_TRACE: {
//       fprintf(stderr, "   [%s] %s:%d\n", module, message, line);
//       break;
//     }
//   }
// }


static void wren_errorFn(
  WrenVM* W, WrenErrorType type, const char* module,
  int32_t line, const char* message
) {
  UNUSED(W);
  switch (type) {
    case WREN_ERROR_COMPILE:
      fprintf(stderr, "[%s line %d] %s\n", module, line, message);
      break;
      
    case WREN_ERROR_RUNTIME:
      fprintf(stderr, "%s\n", message);
      break;
      
    case WREN_ERROR_STACK_TRACE:
      fprintf(stderr, "[%s line %d] in %s\n", module, line, message);
      break;
  }
}

bool hasSuffix(const char *str, const char *suffix) {
    if (!str || !suffix) {
      return false;
    }
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr) {
      return false;
    }
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}


static char *wren_loadModuleFn(WrenVM *W, const char *name) {
  map_str_t *map = wrenGetBuiltins(W);
  
  const char **module = map_get(map, name);

  if (module) {
    return (char *)*module;
  }

  char *result = "";
  if (!hasSuffix(name, ".wren")) {
    char *filename = concat(name, ".wren", NULL);
    result = fs_read(filename, NULL);
    free(filename);
  } else {
    result = fs_read(name, NULL);
  }
  return result;
}


static WrenForeignMethodFn wren_bindForeignMethod(
	WrenVM* W, const char* module, const char* className,
  bool isStatic, const char* signature
) {
  char *fullSignature = concat(module, className, signature, isStatic ? "s" : "", NULL);
	WrenForeignMethodFn_Map *map = wrenGetMethodMap(W);
	WrenForeignMethodFn *method = map_get(map, fullSignature);
	free(fullSignature);
	return method ? *method : NULL;
}


static WrenForeignClassMethods wren_bindForeignClass(
  WrenVM* W, const char* module, const char* className
) {
  char *fullSignature = concat(module, className, NULL);
  WrenForeignClassMethods_Map *map = wrenGetClassMap(W);
  WrenForeignClassMethods *methods = map_get(map, fullSignature);
  free(fullSignature);
  return methods ? *methods : (WrenForeignClassMethods){ NULL, NULL};
}