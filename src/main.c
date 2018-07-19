/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/config.h"
#include "util/util.h"
#include "util/fs.h"
#include "modules/m_os.h"

static WrenVM *W;
static ForeignWrenData foreignData;
static WrenConfiguration configuration;

static void shutdown(void) {
  wrenFreeVM(W);
  map_deinit(&(foreignData.methods));
  map_deinit(&(foreignData.classes));
  map_deinit(&(foreignData.builtins));
}

void wren_open_nest(WrenVM *W);

int32_t main(int32_t argc, const char *argv[]) {
  atexit(shutdown);

#if defined(_WIN32) && !defined(NEST_RELEASE)
  setvbuf(stdout, NULL, _IONBF, BUFSIZ);
#endif

  /* Init our foreign data for Wren */

  map_init(&foreignData.methods);
  map_init(&foreignData.classes);
  map_init(&foreignData.builtins);

  /* Configure Wren VM */
  wrenInitConfiguration(&configuration);

  configuration.userData = &foreignData;
  configuration.writeFn = wren_writeFn;
  configuration.errorFn = wren_errorFn;
  configuration.loadModuleFn = wren_loadModuleFn;
  configuration.bindForeignMethodFn = wren_bindForeignMethod;
  configuration.bindForeignClassFn = wren_bindForeignClass;

  /* Init Wren VM */
  W = wrenNewVM(&configuration);

  /* Init main module -- this also inits the submodules */
  wren_open_nest(W);

  /* Pass in command line arguments */
  w_os_setArguments(argc, argv);

  /* Init embedded scripts -- these should be ordered in the array in the order
   * we want them loaded; init.lua should always be last since it depends on
   * all the other modules */
  #include "nest_wren.h"
  #include "fs_wren.h"
  #include "os_wren.h"

  struct {
    const char *module, *source; int32_t size;
  } builtins[] = {
    { "fs",   fs_wren,   sizeof(fs_wren) },
    { "os",   os_wren,   sizeof(os_wren) },
    { NULL, NULL, 0 }
  };

  /* Register builtin modules */
  for (int32_t i = 0; builtins[i].module; i++) {
    map_set(&foreignData.builtins, builtins[i].module, builtins[i].source);
  }

  struct {
    const char *module, *source; int32_t size;
  } items[] = {
    { "main", nest_wren, sizeof(nest_wren) },
    { NULL, NULL, 0 }
  };

  // printf("--START--\n");

  for (int32_t i = 0; items[i].module; i++) {
    int32_t err = wrenInterpret(W, items[i].module, items[i].source);
    if (err != WREN_RESULT_SUCCESS) abort();
    // if (err == WREN_RESULT_COMPILE_ERROR) exit(65); // EX_DATAERR.
    // if (err == WREN_RESULT_RUNTIME_ERROR) exit(70); // EX_SOFTWARE.
  }

  return 0;
}