/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/modules/Attic/tell.c,v 1.1.2.2 1998/08/06 06:24:35 tdo Exp $

  TELL.C

  A module to implement the tell command
  ***************************************/

#include "../mods.h"
#include <string.h>

typedef struct tell_list {
  char *nick;
  char *msg;
  char *from;
  struct tell_list *prev;
  struct tell_list *next;
} TellList;

TellList *TellHead;

int ModInit (void *id, void *main) {
  int (*ah)(int, void *, char *);
  int (*ac)(void *, char *, char *, int);

  if (main == NULL) {
    fprintf(stderr, "Error in dlopen(NULL, RTLD_LAZY).\n");
    fprintf(stderr, "dlerror: %s\n", dlerror());
    return 0;
  }
  ah = (int (*)(int, void *, char *))dlsym(main, "_AddHook");
  if (ah == NULL) {
    fprintf(stderr, "Error in dlsym(main, \"AddHook\")\n");
    fprintf(stderr, "dlerror: %s\n", dlerror());
    return 0;
  }
  ac = (int (*)(void *, char *, char *, int))dlsym(main, "_AddCommand");
  if (ac == NULL) {
    fprintf(stderr, "Error in dlsym(main, \"AddCommand\")\n");
    fprintf(stderr, "dlerror: %s\n", dlerror());
    return 0;
  }
  
  ModInfoStr = "Tell - The first module";
  ModByStr = "By Chad Walker, V0.5";
  
  TellHead = NULL;
  if ((*ah) (ON_SAY, id, "TellOnSay") == 0) {
    ModErrStr = "AddHook (ON_SAY, id, \"TellOnSay\") failure";
    return 0;
  }
  if ((*ac) (id, "tell", "c_tell", 25) == 0) {
    ModErrStr = "AddCommand (id, \"tell\", \"c_tell\", 25) failure";
    return 0;
  }

  return 1;
}

void TellOnSay (char fromnick[], char msg[]) {
  TellList *tp, *dp = NULL;
  char buffer[MAX_STR_LEN];
  
  tp = TellHead;
  while (tp != NULL) {
    if (!strcmp(tp->nick, fromnick)) {
      sprintf (buffer, "%s told me to tell you:", tp->from);
      notice (fromnick, buffer);
      notice (fromnick, tp->msg);
      if (tp->prev != NULL)
	tp->prev->next = tp->next;
      if (tp->next != NULL)
	tp->next->prev = tp->prev;
      if (tp == TellHead)
	TellHead = tp->next;
      tp->next = dp;
      dp = tp;
    }
    tp = tp->next;
  }

  while (dp != NULL) {
    tp = dp;
    dp = dp->next;
    free (tp);
  }
}

void c_tell (char fromnick[], int level, char arg[], int gender) {
  char nick[10];
  char *tmp;
  char buffer[MAX_STR_LEN];
  TellList *tp;
  
  tmp = strtok (arg, " \t");
  strncpy (nick, tmp, 10);
  nick[9] = '\0';

  switch (checkonchannel(nick, 0)) {
  case 0:  sprintf (buffer, "%s isn't on %s.", nick, mychan);
           notice (fromnick, buffer);
	   break;
  case 1:  tp = (TellList *)malloc(sizeof(TellList));
           tp->nick = nick;
	   tp->from = fromnick;
	   tp->msg = strdup(strtok(NULL,""));
	   tp->prev = NULL;
	   tp->next = TellHead;
	   TellHead = tp;
           break;
  default: sprintf (buffer, "Ya know, I don't think %s is on irc.", nick);
           notice (fromnick, buffer);
  }
}
