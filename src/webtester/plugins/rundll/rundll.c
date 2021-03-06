/**
 * WebTester Server - server of on-line testing system
 *
 * This plugin is needed only for initialization/ubinitialization stuff
 * for LibRUN.
 *
 * Copyright 2008 Sergey I. Sharybin <g.ulairi@gmail.com>
 *
 * This program can be distributed under the terms of the GNU GPL.
 * See the file COPYING.
 */

#include <libwebtester/plugin-defs.h>
#include <libwebtester/core.h>
#include <libwebtester/hook.h>

#include <librun/run.h>

#include "rundll.h"

/**
 * Plugin activation callback
 *
 * @return zero on success, non-zero otherwise
 */
static int
activate (void *__unused, void *__call_unused)
{
  /* Initialize profiling stuff */

  if (run_init ())
    {
      core_print (MSG_INFO, "    **** Error initializing LibRUN. "
                            "Profiling is UNAVALIABLE.\n");
      return -1;
    }

  core_print (MSG_INFO, "    **** LibRUN profiling library is now activated.\n");
  return 0;
}

/**
 * Plugin deactivation callback
 *
 * @return zero on success, non-zero otherwise
 */
static int
deactivate (void *__unused, void *__call_unused)
{
  run_done ();
  core_print (MSG_INFO, "    **** Profiling throught LibRUN "
                        "is now unavaliable.\n");
  return 0;
}

/**
 * Initialize plugin
 *
 * @param __plugin - plugin descriptor
 * @return zero on success, non-zero otherwise
 */
static int
Init (plugin_t *__plugin)
{
  hook_register (CORE_ACTIVATE, activate, 0, HOOK_PRIORITY_NORMAL);
  hook_register (CORE_DEACTIVATE, deactivate, 0, HOOK_PRIORITY_NORMAL);

  return 0;
}

/**
 * Unload plugin
 *
 * @param __plugin - plugin descriptor
 * @return zero on success, non-zero otherwise
 */
static int
OnUnload (plugin_t *__plugin)
{
  hook_unregister (CORE_ACTIVATE, activate, HOOK_PRIORITY_NORMAL);
  hook_unregister (CORE_DEACTIVATE, deactivate, HOOK_PRIORITY_NORMAL);
  return 0;
}

/****
 * Plugin info struct
 */
static plugin_info_t Info = {
  RUNDLL_MAJOR_VERSION,
  RUNDLL_MINOR_VERSION,

  0,
  OnUnload,
  0,
  0
};

PLUGIN_INIT (RUNDLL_LIBNAME, Init, Info);
