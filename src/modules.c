/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/Attic/modules.c,v 1.1.2.3 1998/08/02 22:36:38 tdo Exp $

  MODULES.C

  This file has AddHook() and like functions
  ***************************************/


#include "modules.h"

int AddHook (int event, void *id, char *func)
{
  EventList *tp;
  switch(event)
    {
    case ON_JOIN:    tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnJoin;
		     OnJoin = tp;
		     break;
    case ON_LEAVE:   tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnLeave;
		     OnLeave = tp;
		     break;
    case ON_QUIT:    tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnQuit;
		     OnQuit = tp;
		     break;
    case ON_KICK:    tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnKick;
		     OnKick = tp;
		     break;
    case ON_SAY:     tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnSay;
		     OnSay = tp;
		     break;
    case ON_ME:      tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnMe;
		     OnMe = tp;
		     break;
    case ON_NOTICE:  tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnNotice;
		     OnNotice = tp;
		     break;
    case ON_CNOTICE: tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
  		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnCNotice;
		     OnCNotice = tp;
		     break;
    case ON_MODE:    tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnMode;
		     OnMode = tp;
		     break;
    case ON_TOPIC:   tp = (EventList *)malloc(sizeof(EventList));
                     tp->id = id;
		     tp->func = func;
		     tp->prev = NULL;
		     tp->next = OnTopic;
		     OnTopic = tp;
		     break;
    default:         return 0;
    }
  return 1;
}

int RemoveHook(int event, void *id, char *func)
{
  EventList *tp;
  EventList **head;
  int count = 0;
  
  switch (event)
    {
    case ON_JOIN:    tp = OnJoin;
                     break;
    case ON_LEAVE:   tp = OnLeave;
                     break;
    case ON_QUIT:    tp = OnQuit;
                     break;
    case ON_KICK:    tp = OnKick;
                     break;
    case ON_SAY:     tp = OnSay;
                     break;
    case ON_ME:      tp = OnMe;
                     break;
    case ON_NOTICE:  tp = OnNotice;
                     break;
    case ON_CNOTICE: tp = OnCNotice;
                     break;
    case ON_MODE:    tp = OnMode;
                     break;
    case ON_TOPIC:   tp = OnTopic;
                     break;
    defaut:          return 0;
    }
  head = &tp;
  while (tp != NULL) {
    if ((tp->id == id) && (!strcmp(tp->func, func))) {
      if (tp->next != NULL)
	tp->next->prev = tp->prev;
      if (tp->prev != NULL)
	tp->prev->next = tp->next;
      if (*head == tp)
	*head = tp->next;
      free (tp);
      count++;
    }
    tp = tp->next;
  }

  return count;
}

int AddCommand (void *id, char *cmd, char *func, int level)
{
  CmdList *tp;
  tp = (CmdList *)malloc(sizeof(CmdList));
  tp->id = id;
  tp->cmd = cmd;
  tp->func = func;
  tp->level = level;
  tp->prev = NULL;
  tp->next = Commands;
  Commands = tp;
  return 1;
}

int RemoveCommand (void *id, char *cmd)
{
  CmdList *tp;
  int count = 0;
  
  tp = Commands;
  while (tp != NULL) {
    if ((tp->id == id) && (!strcmp (tp->cmd, cmd))) {
      if (tp->next != NULL)
	tp->next->prev = tp->prev;
      if (tp->prev != NULL)
	tp->prev->next = tp->next;
      if (tp == Commands)
	Commands = tp->next;
      free (tp);
      count++;
    }
    tp = tp->next;
  }
  return count;
}
