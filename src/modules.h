/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/Attic/modules.h,v 1.1.2.5 1998/08/06 06:23:31 tdo Exp $

  MODULES.H

  This file just has some declarations
  ***************************************/


#ifndef MODULES_H
#define MODULES_H    /*+ To stop multiple inclusions. +*/

#include <dlfcn.h>		/*+ for dlopen and the like +*/
#include "mods-common.h"	/*+ for the #define ON_*'s +*/

int AddHook (int event, void *id, char *func); /*+ Add's a hook to the given event +*/
int RemoveHook (int event, void *id, char *func);	/*+ Remove's a hook +*/
int AddCommand (void *id, char *cmd, char *func, int level); /*+ Add's a command +*/
int RemoveCommand (void *id, char *cmd); /*+ Remove's a command +*/

typedef struct event_list
{
  void *id;
  char *func;
  struct event_list *prev;
  struct event_list *next;
} EventList;

EventList *OnJoin;
EventList *OnLeave;
EventList *OnQuit;
EventList *OnKick;
EventList *OnSay;
EventList *OnMe;
EventList *OnNotice;
EventList *OnCNotice;
EventList *OnMode;
EventList *OnTopic;

typedef struct cmd_list
{
  void *id;
  int level;
  char *cmd;
  char *func;
  struct event_list *prev;
  struct event_list *next;
} CmdList;
CmdList *Commands;

#endif /* MODULES_H */
