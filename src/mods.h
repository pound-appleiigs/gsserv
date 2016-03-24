/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/Attic/mods.h,v 1.1.2.6 1998/08/06 06:23:31 tdo Exp $

  MODS.h

  Header file for modules to include
  ***************************************/


#ifndef MODS_H
#define MODS_H    /*+ To stop multiple inclusions. +*/

#include "mods-common.h"
#include "gsserv.h"
#include "funcs.h"
#include <stdio.h>
#include <dlfcn.h>

char *ModErrStr;		/*+ Error string to return to user +*/
char *ModInfoStr;		/*+ Info String, like the name of the
				    module and version +*/
char *ModByStr;			/*+ Whodunit? +*/


/*++++++++++++++++++++++++++++++++++++++
  Used to add hooks, so when an event happens, a function will get called

  extern int AddHook returns 0 on failure, 1 on success

  int event what event to add a hook to

  void *id the id of the module

  char *func function to run
  ++++++++++++++++++++++++++++++++++++++*/

extern int AddHook (int event, void *id, char *func);


/*++++++++++++++++++++++++++++++++++++++
  Removes a hook from an event

  extern int RemoveHook returns 0 on failure, 1 on success

  int event which event to remove the hook from

  void *id the id of the module

  char *func the function name
  ++++++++++++++++++++++++++++++++++++++*/

extern int RemoveHook (int event, void *id, char *func);


/*++++++++++++++++++++++++++++++++++++++
  Add's a command, kind of a specialized hook, when a user /msg GSServ cmd this function will be called.

  extern int AddCommand returns 0 on failure and 1 on success

  void *id id of the module

  char *cmd command to add

  char *func function to call

  int level the user must be to call
  ++++++++++++++++++++++++++++++++++++++*/

extern int AddCommand (void *id, char *cmd, char *func, int level);


/*++++++++++++++++++++++++++++++++++++++
  Removes a command from the hooks list.

  extern int RemoveCommand returns 0 on failure and 1 on success

  void *id id of the module

  char *cmd command to remove
  ++++++++++++++++++++++++++++++++++++++*/

extern int RemoveCommand (void *id, char *cmd);


/*++++++++++++++++++++++++++++++++++++++
  Called from GSServ for every module loaded. Put initialization stuff in here like AddHook()'s and AddCommand()'s

  int ModInit return 0 on failure and put info in ModErStr, or 1 on success

  void *id the module's ID
  ++++++++++++++++++++++++++++++++++++++*/

int ModInit (void *id, void *main);

#endif /* MODS_H */
