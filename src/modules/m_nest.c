/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include "util/util.h"


void wren_open_fs(WrenVM *W);
void wren_open_os(WrenVM *W);

void wren_open_nest(WrenVM *W) {
 static void (*nestClasses[])(WrenVM*) = {
    /* Modules */
    wren_open_fs,
    wren_open_os,
    NULL
  };

  for (int32_t i = 0; nestClasses[i]; i++) {
    nestClasses[i](W);
  }
}