/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/commands.c,v 1.1.1.1.2.18.2.2 1998/08/06 06:23:30 tdo Exp $

  COMMANDS.C

  this file deals with the interaction between gsserv
  and the users via the PRIVMSG.
  
  ***************************************/

#define INC_COMMAND_C	    /*+ So headers know what file is including them +*/
#include "gsserv.h"
#include "funcs.h"
#include "config.h"
#include "commands.h"
#include "modules.h"
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#undef INC_COMMAND_C

extern char *genstr[];		/*+ A Gender string +*/
extern LIST *head,		/*+ The head of the userlist +*/
            *tail;		/*+ The tail of the user list +*/

#ifndef NULL
#define NULL 0			/*+ Should be defined +*/
#endif

/*+ The different commands that are availible +*/
COMMAND cmdlist[] = {
/*+ help, info, and fun +*/
{"help",              0,         c_help},/*+ the help command +*/
{"info",              0,         c_info}, /*+ compilation info +*/
{"level",             0,         c_level}, /*+ user's level info +*/
{"sex",               0,         c_sex}, /*+ You sick bastard +*/
{"werd",              0,         c_werd}, /*+ WERD +*/
/*+ channel control commands +*/
{"ban",               25,        c_ban},/*+ bans someone from the channel +*/
{"bankick",           25,        c_bankick}, /*+ kicks and bans +*/
{"deop",              25,        c_deop}, /*+ remove op +*/
{"kick",              25,        c_kick}, /*+ just kicks +*/
{"mode",              25,        c_mode}, /*+ generic channel mode +*/
{"op",                25,        c_op},	/*+ add operator status +*/
{"topic",             25,        c_topic}, /*+ change the topic +*/
/*+ quotes and insults +*/
{"dream",             50,        c_dream},/*+ good ol' BDK +*/
{"haywood",           50,        c_haywood}, /*+ bangin' +*/
{"invade",            50,        c_invade}, /*+ Dr. T0M +*/
{"invadem",           50,        c_invadem}, /*+ Auri +*/
{"invadet",           50,        c_invadet}, /*+ Dr. T0M +*/
{"jimmy",             50,        c_jimmy}, /*+ it runs deep +*/
{"wzo",               50,        c_wzo}, /*+ wtf is Woz? +*/
/*+ kicks, bans, and bot behavior +*/
{"bitchslap",         75,        c_bitchslap},/*+ alt. kick +*/
{"destroy",           75,        c_destroy}, /*+ alt. kick-ban +*/
{"ldestroy",          75,        c_ldestroy}, /*+ alt. kick-ban +*/
{"me",                75,        c_me},	/*+ does a /me +*/
{"msg",               75,        c_msg}, /*+ sends a priv msg +*/
{"say",               75,        c_say}, /*+ talks +*/
/*+ admin +*/
{"add",               100,       c_add},/*+ Add a user +*/
{"addex",             100,       c_addex},/*+ Add a user +*/
{"goto",              100,       c_goto}, /*+ sends 'serv to a channel +*/
{"join",              100,       c_join}, /*+ goto's the home channel +*/
{"list",              100,       c_list}, /*+ lists users +*/
{"natemode",          100,       c_natemode}, /*+ toggles NATEMODE +*/
{"nick",              100,       c_nick}, /*+ changes 'serv's nick +*/
{"opall",             100,       c_opall}, /*+ OP's all registered users +*/
{"quote",             100,       c_quote}, /*+ ??? +*/
{"reload",            100,       c_reload}, /*+ reloads the userlist +*/
{"remove",            100,       c_remove}, /*+ remove a user +*/
{"shutdown",          100,       c_shutdown}, /*+ shuts 'serv down +*/
{"status",            100,       c_status}, /*+ shows some stats +*/
{"writefile",         100,       c_writefile} /*+ writes the user file +*/
};

/*++++++++++++++++++++++++++++++++++++++
  main function to find/execute command. woot.

  
  int do_command returns:
    0 - unknown command or level too low
    1 - command okay

  char cmdname[] -- which command to run

  char fromnick[] -- who's it from?

  int level -- What's their level?

  char arg[] -- the arguments to the command

  int gender -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

int do_command(char cmdname[], char fromnick[], int level, char arg[],
               int gender)
{
  static numcmds = sizeof(cmdlist) / sizeof(COMMAND);
  int cmdcounter, retval = 0;
  CmdList *tp;
  void (*modfunc) (char fromnick[], int level, char arg[], int gender);

  for (cmdcounter = 0; cmdcounter < numcmds ; cmdcounter++)
  {
    if (strcasecmp(cmdlist[cmdcounter].cmdname, cmdname) == 0) 
    {
      if (level >= cmdlist[cmdcounter].minlevel)
      {
	/* execute the command and return with the o.k. */
        (*cmdlist[cmdcounter].func) (fromnick, level, arg, gender);
	retval = 1;
	break;
      }
      else
      {
        /* return with the "fuckoff" */
        retval = 0;
	break;
      }
    }
  }

  if (retval == 0) {
    tp = Commands;
    while ((tp != NULL) && (retval == 0)) {
      if (!strcmp (cmdname, tp->cmd) && (tp->level <= level)) {
	modfunc = (void (*)(char fromnick[], int level, char arg[], int gender))dlsym (tp->id, tp->func);
	if (modfunc != NULL) {
	  (*modfunc) (fromnick, level, arg, gender);
	  retval = 1;
	}
      }
      tp = tp->next;
    }
  }
  return(retval);
}


/*++++++++++++++++++++++++++++++++++++++
  user asked for their level; give it to them

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_level (char fromnick[], int level, char arg[], int gend)
{
  int otherlevel;
  char user[128], host[256];
  char buf[MAX_STR_LEN];

  if (arg[0] == '\0')  /* get my level */
  {
    sprintf (buf, "Your current %s level is %d", mynick, level);
    notice (fromnick, buf);
  }
  else  /* get other's level */
  {
    switch (checkonchannel (arg, 0))
    {
      case 0:
        sprintf (buf, "Look around, %s isn't on this channel!", arg);
        notice (fromnick, buf);
        break;

      case 1:
        getuserhost (arg, user, host);
        otherlevel = getlevel (arg, user, host, &gend);
        sprintf (buf, "%s's current %s level is %d", arg, mynick, otherlevel);
        notice (fromnick, buf);
        break;

      default:
        sprintf (buf, "%s is no where to be found on IRC!", arg);
        notice (fromnick, buf);
        break;
    }
  }
}


/*++++++++++++++++++++++++++++++++++++++
  user asked for help

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_help (char fromnick[], int level, char arg[], int gend)
{
  static numcmds = sizeof(cmdlist) / sizeof(COMMAND);
  int cmdcounter, helpcmdlist = 0;
  char buf[MAX_STR_LEN];

  for (cmdcounter = 0; cmdcounter < numcmds ; cmdcounter++)
  {
    if (arg[0] == '\0')
    {
      printhelp (fromnick, level, helpcmdlist);
      break;
    }
    else
    {
      if (strcasecmp(cmdlist[cmdcounter].cmdname, arg) == 0)
      {
        if (level >= cmdlist[cmdcounter].minlevel)
        {
          /* execute the help command */
          printhelp (fromnick, level, cmdcounter);
          break;
        }
        else
        {
          sprintf (buf, "*** %s: unknown command or access level too low.",
                   arg);
          notice (fromnick, buf);
          sprintf (buf, "*** type \"/MSG %s HELP\" for a list of authorized"
                   " commands", mynick);
          notice (fromnick, buf);
          break;
        }
      }
    }
  }
}


/*++++++++++++++++++++++++++++++++++++++
  user asked for gsserv info

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_info (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

#ifndef NeXT
  struct utsname myutsname;
  char *os;
  char *release;
  char *platform;

  uname (&myutsname);
  os = myutsname.sysname;
  release = myutsname.release;
  platform = myutsname.machine;

  sprintf (buf, "I am running on %s %s %s", os, release, platform);
  notice (fromnick, buf);
#else
  /* we don't need no steenkin posix apis! */
  notice (fromnick, "I am running on OPENSTEP/Mach 4.2 m68k");
#endif

/*   sprintf (buf, "and was created by %s@%s", BUILD_USER, BUILD_HOSTNAME); */
  sprintf (buf, "This is build %d, made by %s@%s", BUILD_NUMBER, BUILD_USER,
	   BUILD_HOSTNAME);
  notice (fromnick, buf);

  sprintf (buf, "built at %s on %s.", __TIME__, __DATE__);
  notice (fromnick, buf);
  sprintf (buf, "I was started at: %s.", ctime (&starttime));
  notice (fromnick, buf);
}


/*++++++++++++++++++++++++++++++++++++++
  sick bastard

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/o
  ++++++++++++++++++++++++++++++++++++++*/

void c_sex (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  sprintf (buf, "PRIVMSG %s :%s is a sick bastard who wants to have sex with me!", mychan, fromnick);
  writeline (buf);
}


/*++++++++++++++++++++++++++++++++++++++
  add a user to the userlist

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_add (char fromnick[], int level, char arg[], int gend)
{
  char nick[10];
  char user[50];
  char host[100];
  int lev;
  int gen;
  char buffer[MAX_STR_LEN];
  int x;
  int found = 0;
  
  if (arg[0] == '\0') {
    notice (fromnick, "Perhaps you should specify who to add.");
    return;
  }
  sscanf (arg, "%s %d %d", nick, &lev, &gen);
  switch (checkonchannel (nick, 0))
    {
    case 0:
    case 1:
      getuserhost (nick, user, host);
      for (x = strlen(host) - 1; (found < 2) && (x > 0); x--)
	if (host[x] == '.')
	  found++;
      x++;
      if (host[x] == '.')
	x++;
      to_lower(host + x);
      sprintf (buffer, "%s %s %s$ %d %d", nick, user, (host + x), lev, gen);
      c_addex (fromnick, level, buffer, gend);
      break;
    default:
      sprintf (buffer, "%s doesn't appear to be on irc.", nick);
      notice (fromnick, buffer);
    }
}


/*++++++++++++++++++++++++++++++++++++++
  add a user to the userlist (Using extended notation)

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_addex (char fromnick[], int level, char arg[], int gend)
{
  char nick[10], uname[50], hname[100];
  int level, gend;
  char temp[200];
  
  sscanf (arg, "%s %s %s %d %d", nick, uname, hname, &level, &gend);
  addentry (nick, uname, hname, level, gend);
  sprintf (temp, "Added nick=%s uname=%s hname=%s level=%d gend=%d to the user list.", nick, uname, hname, level, gend);
  notice (fromnick, temp);
}


/*++++++++++++++++++++++++++++++++++++++
  remove a user from the userlist

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_remove (char fromnick[], int level, char arg[], int gend)
{
  LIST *listptr;
  LIST *tp;
  char user[50];
  char host[100];
  char buffer[1024];
  int found = 0;

  switch (checkonchannel(arg, 0))
    {
    case 0:
    case 1:
      getuserhost(arg, user, host);
      to_lower (user);
      to_lower (host);
      tp = (LIST *) malloc(sizeof(LIST));
      tp->next = head;
      listptr = tp;
      while ((!found) && (listptr->next != NULL)) {
	if (!strcasecmp (listptr->next->uname, user) ||
	    (!strcmp (listptr->next->uname, "*"))) {
	  re_comp (listptr->next->hname);
	  if (re_exec (host) == 1)
	    found = 1;
	}
	if (!found) listptr = listptr->next;
      }
      if (!found) {
	sprintf (buffer,"%s is not in my user list (at least at %s@%s).",
		 arg, user, host);
	notice (fromnick, buffer);
	notice (fromnick, "Nothing done.");
	return;
      }
      if (tp == listptr) {
	head = head->next;
	listptr = tp;
	tp = tp->next;
	free (listptr);
      } else {
	free (tp);
	tp = listptr->next;
	listptr->next = tp->next;
      }
      DIRTYUFILE = 1;
      sprintf (buffer, "%s (%s@%s) has been removed from the userlist.",
	       arg, tp->uname, tp->hname);
      notice (fromnick, buffer);
      free (tp);
      return;
    default:
      sprintf (buffer, "Ya know, I don't think %s is on irc.", arg);
      notice (fromnick, buffer);
      notice (fromnick, "Nothing done.");
    }
}


/*++++++++++++++++++++++++++++++++++++++
  user-requested deop

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_deop (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (checkonchannel (fromnick, level) == 1)
  {
    if (arg[0] == '\0')
      notice (fromnick, "Who do you want to deop??");
    else
    {
      if (strcasecmp (arg, mynick) == 0)
        notice (fromnick, "No way! I like being op!");
      else
      {
        switch (checkonchannel (arg, 0))
        {
	  case 0:
            sprintf (buf, "Well, %s isn't on this channel!", arg);
            notice (fromnick, buf);
	    break;

	  case 1:
            sprintf (buf, "%s wants me to deop %s", fromnick, arg);
            notice (mychan, buf);
            sprintf (buf, "-o %s", arg);
            mode (buf);
	    break;

	  default:
            sprintf (buf, "Ya know, I don't think %s is even _ON_ IRC!", arg);
            notice (fromnick, buf);
	    break;
        }
      }
    }
  }
  else
  {
    sprintf (buf, "But you're not _on_ %s!!", mychan);
    notice (fromnick, buf);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  join a channel. woo woo!

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_join (char fromnick[], int level, char arg[], int gend)
{
  part (mychan);
  join (HOMECHAN);
}


/*++++++++++++++++++++++++++++++++++++++
  coid c_kick -- kick someone off the channel

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_kick (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (checkonchannel (fromnick, level))
  {
    if (arg[0] == '\0')
      notice (fromnick, "Who do you wanna kick?");
    else
    {
      if (strcasecmp (arg, mynick) == 0)
        notice (fromnick, "Hey! I'm not that stupid!");
      else
      {
        switch (checkonchannel (arg, 0))
        {
	  case 0:
            sprintf (buf, "Just So You Know: %s isn't on %s.", arg, mychan);
            notice (fromnick, buf);
            break;
        
	  case 1:
            sprintf (buf, "%s wants %s kicked", fromnick, arg);
            notice (mychan, buf);
            kick (arg, "\0");
            break;

	  default:
            sprintf (buf, "Heh... no need to do that! %s isn't on IRC!", arg);
            notice (fromnick, buf);
            break;
        }
      }
    }
  }
  else
  {
    notice (fromnick, "Ya know, it's really rude to kick someone off of "
              "channel that");
    notice (fromnick, "you aren't even on!");
  }
}


/*++++++++++++++++++++++++++++++++++++++
  list all users

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments?

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_list (char fromnick[], int level, char arg[], int gend)
{
  notice (fromnick, "LIST currently not implemented");
}


/*++++++++++++++++++++++++++++++++++++++
  request a mode change?

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_mode (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (checkonchannel (fromnick, level))
  {
    if (arg[0] == '\0')
      notice (fromnick, "What do you want to change the channel mode to?");
    else
      sprintf (buf, "%s wants me to change the mode on %s to %s", fromnick,
               mychan, arg);
    notice (mychan, buf);
    mode (arg);
  }
  else
  {
    sprintf (buf, "Try to be on %s before changing it's mode!", mychan);
    notice (fromnick, buf);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  make someone an operator, like there are those that aren't..

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_op (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (checkonchannel (fromnick, level))
  {
    if (arg[0] == '\0')
    {
      sprintf (buf, "%s wants me to make %s chanop", fromnick, genstr[gend]);
      if(level < 100)
	notice (mychan, buf);
      sprintf (buf, "+o %s", fromnick);
      mode (buf);
    }
    else
    {
      switch (checkonchannel (arg, 0))
      {
        case 0:
          sprintf (buf, "But.... but.... %s isn't on %s!", arg, mychan);
          notice (fromnick, buf);
          break;

        case 1:
          sprintf (buf, "%s wants me to op %s", fromnick, arg);
	  if(level < 100)
	    notice (mychan, buf);
          sprintf (buf, "+o %s", arg);
          mode (buf);
          break;

        default:
          sprintf (buf, "Can't do that 'cuz %s isn't on IRC!", arg);
          notice (fromnick, buf);
          break;
      }
    }
  }
  else
    notice (fromnick, "Sorry dude, but I can't op you if you aren't here...");
}


/*++++++++++++++++++++++++++++++++++++++
  uh huh

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_jimmy (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  sprintf (buf, "PRIVMSG %s :o/` my jimmy", mychan);
  writeline (buf);
  sprintf (buf, "PRIVMSG %s :my jimmy run so deep", mychan);
  writeline (buf);
  sprintf (buf, "PRIVMSG %s :run so deep it puts her butt ta sleep o/`", mychan);
  writeline (buf);
}


/*++++++++++++++++++++++++++++++++++++++
  coid c_haywood -- me and your .o files bang for hours

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_haywood (char fromnick[], int level, char arg[], int gend)
{
  int i;
  char temp[1024];

  i = (int) ((((double)random ())*((double)NUM_HAYWOODS)) / (RAND_MAX + 1.0));

#ifdef DEBUG
  printf("c_haywood(): doing haywood number %d\n", i);
#endif
  if (arg && (*arg != '\0'))
    sprintf(temp,"PRIVMSG %s :Hey %s, %s", mychan, arg, jablowme[i]);
  else
    sprintf(temp,"PRIVMSG %s :%s",mychan, jablowme[i]);

  writeline(temp);
}


/*++++++++++++++++++++++++++++++++++++++
  young snatch

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_dream (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  sprintf (buf, "PRIVMSG %s :<Plaque> i had a dream where i enrolled back "
         "in grade school so i could get some young snatch", mychan);
  writeline (buf);
}


/*++++++++++++++++++++++++++++++++++++++
  that crazy old shit

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/Ciannait
  ++++++++++++++++++++++++++++++++++++++*/

void c_wzo (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  sprintf (buf, "PRIVMSG %s :<Plaque> question for the laydeez", mychan);
  writeline (buf);
  sprintf (buf, "PRIVMSG %s :<Plaque> if you had the choose between sex "
           "with Woz or Bill Gates, who wouldya choose?", mychan);
  writeline (buf);
  sprintf (buf, "PRIVMSG %s :<Ciannait> Wzo?", mychan);
  writeline (buf);
  sprintf (buf, "PRIVMSG %s :<Ciannait> who the fuck is Woz?", mychan);
  writeline (buf);
}


/*++++++++++++++++++++++++++++++++++++++
  do topic shit, of course

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_topic (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (arg[0] == '\0')
    notice (fromnick, "Set the topic to what?");
  else
  {
    sprintf (buf, "TOPIC %s :%s", mychan, arg);
    writeline (buf);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  say shit

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_say (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (arg[0] == '\0')
    notice (fromnick, "Say what?");
  else
  {
    sprintf (buf, "PRIVMSG %s :%s", mychan, arg);
    writeline (buf);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  * GSS licks Emmy

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_me (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (arg[0] == '\0')
    notice (fromnick, "Sorry, I can't read your mind.  Give a parameter.");
  else
  {
    sprintf (buf, "PRIVMSG %s :ACTION %s", mychan, arg);
    writeline (buf);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  private messages. toot toot.

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_msg (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];
  char *tp1, *tp2;

  if (arg[0] == '\0')
    sprintf (fromnick, "I can't read your mind, dork.");
  else
  {
    tp1 = strtok (arg, " ");
    tp2 = strtok (NULL, "\n");
    if (!strcasecmp(tp1, mynick)) {
      notice (fromnick, "I ain't gonna send a message to myself.");
      return;
    }
    sprintf (buf, "PRIVMSG %s :%s", tp1, tp2);
    writeline (buf);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  RECTAL "4D": INVASION.

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_invade (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN], *tp;

  if (arg[0] == '\0')
    notice (fromnick, "God, can't even use INVADE right.  No anus for you!");
  else
  {
    tp = arg;

    while (*tp++)
      *tp = toupper (*tp);
    *arg = toupper (*arg);

    sprintf (buf, "PRIVMSG %s :ATTENTION: %s", mychan, arg);
    writeline (buf);
    sprintf (buf, "PRIVMSG %s :YOU: WILL BE; RAMMED (RAMMID): UP THE \"ASS.\"",
             mychan);
    writeline (buf);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  man can that yoshi choke it down

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_invadem (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN], *tp;

  if (arg[0] == '\0')
    notice (fromnick, "EEK! invade yourself!");
  else
  {
    tp = arg;
    while (*tp++)
      *tp = toupper (*tp);
    *arg = toupper (*arg);

    sprintf (buf, "PRIVMSG %s :ATTENTION: %s", mychan, arg);
    writeline (buf);
    sprintf (buf, "PRIVMSG %s :YOU: WILL BE; RAMMED (RAMMID): IN THE "
             "\"MOUTH.\" (AKA AURI-LLY)", mychan);
    writeline (buf);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  BONDAGE (AND YOU WILL)

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments?

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_invadet (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN], *tp;

  if (arg[0] == '\0')
    notice (fromnick, "you sicko! invade yourself!");
  else
  {
    tp = arg;

    while (*tp++)
      *tp = toupper (*tp);

    *arg = toupper (*arg);

    sprintf (buf, "PRIVMSG %s :ATTENTION: %s", mychan, arg);
    writeline (buf);
    sprintf (buf, "PRIVMSG %s :YOU: WILL BE; TIED UP WITH, 9D ROPE, "
             "THEN; RAMMED", mychan);
    writeline (buf);
    sprintf (buf, "PRIVMSG %s :UP THE \"ASS\".", mychan);
    writeline (buf);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  certain death

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments?

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_shutdown (char fromnick[], int level, char arg[], int gend)
{
  quit (fromnick);
}


/*++++++++++++++++++++++++++++++++++++++
  Shows the status of: DIRTYUFILE and NATEMODE

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_status (char fromnick[], int level, char arg[], int gend)
{
  char temp[257];
  int numusers = 0;
  LIST *tp;
  long diff = time (NULL) - starttime;
  long sec = diff % 60;
  long min;
  long hours;
  long days;
  long years;

  diff -= sec;
  diff /= 60;
  min = diff % 60;
  diff /= 60;
  hours = diff % 24;
  diff /= 24;
  days = diff % 365;
  years = diff /365;
  
  tp = head;
  while (tp != NULL) {
    tp = tp->next;
    numusers++;
  }
  notice (fromnick, "GSServ's status:");
  notice (fromnick, "--------------------");
  sprintf (temp, "Userfile is: %s\tNateMode is: %s",
	   (DIRTYUFILE)?"dirty":"clean", (NATEMODE)?"enabled":"disabled");
  notice (fromnick, temp);
  sprintf (temp, "I currently have %d registered users.", numusers);
  notice (fromnick, temp);
  sprintf (temp, "I was started on: %s", ctime (&starttime));
  notice (fromnick, temp);
  if (years)
    sprintf (temp, "I have been running for %d years, %d days, %d:%d:%d.",
	     years, days, hours, min, sec);
  else if (days)
    sprintf (temp, "I have been running for %d days, %d:%d:%d.",
	     days, hours, min, sec);
  else if (hours)
    sprintf (temp, "I have been running for %d:%d:%d", hours, min, sec);
  else
    sprintf (temp, "I have been running for %d:%d", min, sec);
  notice (fromnick, temp);
}


/*++++++++++++++++++++++++++++++++++++++
  Writes the user file to disk, if needed

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_writefile (char fromnick[], int level, char arg[], int gend)
{
  DIRTYUFILE = 1;
  checkufile ();
  notice (fromnick, "User file writen.");
}


/*++++++++++++++++++++++++++++++++++++++
  put shit directly to the server

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_quote (char fromnick[], int level, char arg[], int gend)
{
  writeline (arg);
}


/*++++++++++++++++++++++++++++++++++++++
  join another channel

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_goto (char fromnick[], int level, char arg[], int gend)
{
  if (arg[0] == '\0')
    notice (fromnick, "Where to, idiot?");
  else
    join (arg);
}


/*++++++++++++++++++++++++++++++++++++++
  mega-destructo mode!

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_destroy (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (checkonchannel (fromnick, level))
  {
    if (arg[0] == '\0')
      notice (fromnick, "Who do you want to destroy?");
    else
    {
      if (strcasecmp (arg, mynick) == 0)
        notice (fromnick, "Destroy myself? Shyyyeah, right!");
      else
      {
        switch (checkonchannel (arg, 0))
        {
          case 0:
            sprintf (buf, "Just So You Know: %s isn't on %s.", arg, mychan);
            notice (fromnick, buf);
            break;

          case 1:
            sprintf (buf, "Mega-Destructo Mode activated against %s", arg);
            notice (mychan, buf);
            ban (arg);
            kick (arg, "You are a victim of Mega-Destructo Mode!  Ha!");
            sprintf (buf, "PRIVMSG %s :%s", arg, "Na na na na, na na na na, hey hey hey, good-bye!");
            writeline (buf);
            break;

          default:
            sprintf (buf, "Heh... no need to do that! %s isn't on IRC!", arg);
            notice (fromnick, buf);
            break;
        }
      }
    }
  }
  else
  {
    notice (fromnick, "Ya know, it's really rude to destroy someone off of "
              "channel that");
    notice (fromnick, "you aren't even on!");
  }
}


/*++++++++++++++++++++++++++++++++++++++
  long mega-destructo

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_ldestroy (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (checkonchannel (fromnick, level))
  {
    if (arg[0] == '\0')
      notice (fromnick, "Who do you want to liberal-destroy?");
    else
    {
      if (strcasecmp (arg, mynick) == 0)
        notice (fromnick, "Liberal-destroy myself? Shyyyeah, right!");
      else
      {
        switch (checkonchannel (arg, 0))
        {
          case 0:
            sprintf (buf, "Just So You Know: %s isn't on %s.", arg, mychan);
            notice (fromnick, buf);
            break;

          case 1:
            sprintf (buf, "I'm supposed to destroy %s.  There's an NEA grant for that...", arg);
            notice (mychan, buf);
            sprintf (buf, "Maybe I will, maybe I won't.  Somebody take a poll!");
            notice (mychan, buf);
            sprintf (buf, "Heck, I'm more decisive than Reagan.  I'll go for it!");
            notice (mychan, buf);
            ban (arg);
            kick (arg, "That NEA grant better be big.");
            sprintf (buf, "PRIVMSG %s :%s", arg, "I hope I didn't hurt your feelings too bad.");
            writeline (buf);
            break;

          default:
            sprintf (buf, "Heh... no need to do that! %s isn't on IRC!", arg);
            notice (fromnick, buf);
            break;
        }
      }
    }
  }
  else
  {
    notice (fromnick, "Ya know, it's really rude to liberal destroy someone "
              "off of channel that");
    notice (fromnick, "you aren't even on!");
  }
}


/*++++++++++++++++++++++++++++++++++++++
  change that nick!

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_nick (char fromnick[], int level, char arg[], int gend)
{
  int onchan;
  char buf[MAX_STR_LEN];

  if (arg[0] == '\0')
    notice (fromnick, "Whom?");
  else
  {
    onchan = checkonchannel (arg, 0);
    if ((onchan != 0) && (onchan != 1))
    {
      sprintf (buf, "NICK %s", arg);
      writeline (buf);
      strcpy (mynick, arg);
    }
    else
    {
      sprintf (buf, "Sorry, that nick is in use.");
      notice (fromnick, buf);
    }
  }
}


/*++++++++++++++++++++++++++++++++++++++
  OP's all of the registered users on the channel

  char fromnick[]

  int level

  char arg[]

  int gend
  ++++++++++++++++++++++++++++++++++++++*/

void c_opall (char fromnick[], int level, char arg[], int gend)
{
  char buffer[MAX_STR_LEN];
  char *nick;
  char user[50];
  char host[100];
  int lev;
  int *gen;
  
  sprintf (buffer, "NAMES %s", mychan);
  writeline (buffer);
  readline (buffer);
  nick = strtok (buffer, ":");

  while (nick = strtok (NULL, " \n")) {
    if (nick[0] == '@')
      continue;
    getuserhost(nick, user, host);
    lev = getlevel (nick, user, host, gen);
    if (lev >= 50) {
      sprintf (host, "+o %s", nick); /* why not reuse the host string? */
      mode (host);
    }
  }
}

    
/*++++++++++++++++++++++++++++++++++++++
  re-load the users file

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_reload (char fromnick[], int level, char arg[], int gend)
{
  readusers ();
}


/*++++++++++++++++++++++++++++++++++++++
  bitchslap some honky

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_bitchslap (char fromnick[], int level, char arg[], int gend)
{
  char action[MAX_STR_LEN];
  char reason[MAX_STR_LEN];
  char message[MAX_STR_LEN];
  char buf[MAX_STR_LEN];

  if (checkonchannel (fromnick, level))
  {
    if (arg[0] == '\0')
      notice (fromnick, "Who do you wanna bitchslap?");
    else
    {
      if (strcasecmp (arg, mynick) == 0)
        notice (fromnick, "Like I'd really bitchslap myself. Dweeb!");
      else
      {
        switch (checkonchannel (arg, 0))
        {
          case 0:
            sprintf (buf, "Just So You Know: %s isn't on %s.", arg, mychan);
            notice (fromnick, buf);
            break;

          case 1:
            sprintf (action, "raises his hand high in the air...");
            sprintf (buf, "PRIVMSG %s :ACTION %s", mychan, action);
            writeline (buf);
            sprintf (action, "brings it down across %s's face with a loud slap!", arg);
            sprintf (buf, "PRIVMSG %s :ACTION %s", mychan, action);
            writeline (buf);
            kick (arg, "You have been bitchslapped, sir!");
            sprintf (message, "Yo ass been pimped by dis nigga: %s, word.", fromnick);
            sprintf (buf, "PRIVMSG %s :%s", arg, message);
            writeline (buf);
            break;

          default:
            sprintf (buf, "Heh... no need to do that! %s isn't on IRC!", arg);
            notice (fromnick, buf);
            break;
        }
      }
    }
  }
  else
  {
    notice (fromnick, "Ya know, it's really rude to bitchslap someone off of "
              "channel that");
    notice (fromnick, "you aren't even on!");
  }
}


/*++++++++++++++++++++++++++++++++++++++
  WERD.

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_werd (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  sprintf (buf, "WERD, BITCH.");
  notice (fromnick, buf);
}


/*++++++++++++++++++++++++++++++++++++++
  bans someone

  char fromnick[] -- who's it from

  int level -- what's their leve

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_ban (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (checkonchannel (fromnick, level))
  {
    if (arg[0] == '\0')
      notice (fromnick, "Who do you want me to ban?");
    else
    {
      if (strcasecmp (arg, mynick) == 0)
        notice (fromnick, "Ban myself? You've got to be kidding!");
      else
      {
        switch (checkonchannel (arg, 0))
        {
          case 0:
            sprintf (buf, "Just So You Know: %s isn't on %s.", arg, mychan);
            notice (fromnick, buf);
            break;

          case 1:
            sprintf (buf, "%s wants %s banned", fromnick, arg);
            notice (mychan, buf);
            ban (arg);
            break;

          default:
            sprintf (buf, "Heh... no need to do that! %s isn't on IRC!", arg);
            notice (fromnick, buf);
            break;
        }
      }
    }
  }
  else
  {
    notice (fromnick, "Ya know, it's really rude to ban someone off of "
              "channel that");
    notice (fromnick, "you aren't even on!");
  }
}


/*++++++++++++++++++++++++++++++++++++++
  bankicks someone

  char fromnick[] -- who's it from

  int level -- what's their level

  char arg[] -- any arguments

  int gend -- M/F/N/O
  ++++++++++++++++++++++++++++++++++++++*/

void c_bankick (char fromnick[], int level, char arg[], int gend)
{
  char buf[MAX_STR_LEN];

  if (checkonchannel (fromnick, level))
  {
    if (arg[0] == '\0')
      notice (fromnick, "Who do you want me to ban-kick?");
    else
    {
      if (strcasecmp (arg, mynick) == 0)
        notice (fromnick, "Ban-kick myself? You've got to be kidding!");
      else
      {
        switch (checkonchannel (arg, 0))
        {
          case 0:
            sprintf (buf, "Just So You Know: %s isn't on %s.", arg, mychan);
            notice (fromnick, buf);
            break;

          case 1:
            sprintf (buf, "%s wants %s ban-kicked", fromnick, arg);
            notice (mychan, buf);
            ban (arg);
            kick (arg, "\0");
            break;

          default:
            sprintf (buf, "Heh... no need to do that! %s isn't on IRC!", arg);
            notice (fromnick, buf);
            break;
        }
      }
    }
  }
  else
  {
    notice (fromnick, "Ya know, it's really rude to ban someone off of "
              "channel that");
    notice (fromnick, "you aren't even on!");
  }
}

void c_natemode (char fromnick[], int level, char arg[], int gend)
{
  NATEMODE = !(NATEMODE);
  if(NATEMODE)
    notice (fromnick, "NATEMODE ENABLED");
  else
    notice (fromnick, "NATEMODE DISABLED");
}
