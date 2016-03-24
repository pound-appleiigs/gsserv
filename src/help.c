/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/help.c,v 1.1.1.1.2.9 1998/08/02 05:46:22 tdo Exp $

  HELP.C

  contains all the helpful help text
  
  ***************************************/


#define INC_HELP_C		/*+ So headerfiles know who included them +*/
#include "gsserv.h"
#include "funcs.h"
#undef INC_HELP_C

#include <stdio.h>


/*++++++++++++++++++++++++++++++++++++++
  Print the help

  char from[] -- who's the request from

  int usrlevel -- what's their level

  int number -- which help to print
  ++++++++++++++++++++++++++++++++++++++*/

void printhelp (char from[], int usrlevel, int number)
{
  char buffer[80];

  switch (number)
  {
    case 0:                    /* HELP */
      notice (from, "GSServ: The #AppleIIgs Channel Server");
      notice (from, "Originally by Dave Huang (Calamity).");
      notice (from, "This version contains code by Ian Schmidt (IRSMan),");
      notice (from, "Dave Roberts (daver), Robert Hill (The_Phog),");
      notice (from, "Kevin Brintnall (jalapenio), Mark Miller (markm),");
      notice (from, "and Chad Walker (td0).");
      notice (from, "-------------------------------------------------------");
      sleep (1);
      sprintf (buffer, "To use GSServ, type \"/MSG %s <command>\", where",
               mynick);
      notice (from, buffer);
      notice (from, "command is one of:");
      notice (from, "HELP    INFO    LEVEL   SEX     WERD");
      sleep (1);
      if (usrlevel >= 25)
      {
        notice (from, "BAN     DEOP    KICK    MODE    OP      TOPIC");
      }
      if (usrlevel >= 50)
      {
        notice (from, "DREAM   HAYWOOD INVADE  INVADEM INVADET JIMMY");
        notice (from, "WZO");
        notice (from, "(plus GSServ will auto-op you when you join)");
      }
      if (usrlevel >= 75)
      {
        notice (from, "BITCHSLAP  DESTROY  LDESTROY   ME  MSG  SAY");
      }
      if (usrlevel >= 100)
      {
        notice (from, "ADD   ADDEX  GOTO   JOIN    LIST    NATEMODE");
	notice (from, "NICK  OPALL  QUOTE  RELOAD  REMOVE");
	notice (from, "SHUTDOWN  STATUS  WRITEFILE");
      }
      notice (from, "To get more info on one of those commands, type");
      sprintf (buffer, "\"/MSG %s HELP <command>\"", mynick);
      notice (from, buffer);
      notice (from, "*** End of help");
      break;
    case 1:                    /* INFO */
      notice (from, "Background:");
      notice (from, "   GSServ started out as a 'bot that Dave Huang");
      notice (from, "(khym@bga.com, Calamity on IRC) wrote to");
      notice (from, "see if he knew enough to write a 'bot with IRC");
      notice (from, "commands. It has since grown into a program written");
      notice (from, "entirely in C in order to make it more flexible.");
      notice (from, "The main function of GSServ is to make certain people");
      notice (from, "chanop, but there are a few others included just for");
      notice (from, "fun...");
      sleep (1);
      notice (from, "   Most of the commands that GSServ understands");
      notice (from, "are available only to privileged users, so you won't");
      notice (from, "be able to use 'em unless you're on GSServ's authorized");
      sprintf (buffer, "user's list. (/MSG %s LEVEL to see your userlevel).", mynick);
      notice (from, buffer);
      notice (from, "   BTW, GSServ checks username, and hostname, so");
      notice (from, "don't think you can fool GSServ into thinking that");
      notice (from, "you're authorized just by changing your nick! ;-)");
      sleep (1);
      notice (from, "Release history:");
      notice (from, "  v 1.0 - original release by Dave Huang");
      notice (from, "  v 2.0 - ? (unknown)");
      notice (from, "  v 3.0 - contains significant additional coding");
      notice (from, "          (and new commands and stuff) by Ian Schmidt.");
      notice (from, "  v 4.0 - contains significant code changes and new");
      notice (from, "          commands.  (Dave Roberts)");
      notice (from, "  v 5.0 - reorganized version 3.0 code, implemented");
      notice (from, "          new config file feature, redesigned network");
      notice (from, "          code.  (Robert Hill and Kevin Brintnall)");
      notice (from, "  v 5.1 - added help text, merged some commands from");
      notice (from, "          version 4.0 code, optimizations.  (Mark Miller)");
      notice (from, "  v 5.2 - basic code reorganization. (Chad Walker)");
      sleep (1);
      notice (from, "Mail Chad (tdo@visi.com) if you have problems.");
      notice (from, "Note that Dave, Ian, Dave, Kevin, Rob, Mark and Chad cannot be");
      notice (from, "held responsible if any GSServ users do anything nasty,");
      notice (from, "but Chad will be glad to remove their access if so.");
      notice (from, "*** End of help");
      break;
    case 2:                    /* LEVEL */
      notice (from, "Help on LEVEL");
      notice (from, "Shows the GSServ user level of the specified nick");
      notice (from, "if they are on IRC, or if you don't specify a nick");
      notice (from, "it shows you your GSServ user level.");
      sprintf (buffer, "Usage is /MSG %s LEVEL [nick]", mynick);
      notice (from, buffer);

      break;
    case 3:                    /* SEX */
      sprintf(buffer, "Hey guys, get this: %s doesn't know what sex is! Hahahah!", from);
      notice (mychan, buffer);
      notice (from, "Help on SEX");
      notice (from, "Sex is where you like put your weiner in some");
      notice (from, "chick's thingie.");
      break;
    case 4:                    /* WERD */
      notice (from, "Help on WERD");
      notice (from, "WERD.");
      break;
    case 5:                    /* BAN */
      notice (from, "Help on BAN");
      notice (from, "Bans a user from the channel.");
      notice (from, "Note that some people hack user@host or insert");
      notice (from, "control characters, so this may not stop all lusers.");
      sprintf (buffer, "Usage is /MSG %s BAN <nick>", mynick);
      notice (from, buffer);
      break;
    case 6:                    /* BANKICK */
      notice (from, "Help on BANKICK");
      notice (from, "Ban and kicks a user from the channel.");
      notice (from, "Note that some people hack user@host or insert");
      notice (from, "control characters, so this may not stop all lusers.");
      sprintf (buffer, "Usage is /MSG %s BANKICK <nick>", mynick);
      notice (from, buffer);
      break;
    case 7:                    /* DEOP */
      notice (from, "Help on DEOP");
      notice (from, "It deops someone... duh!");
      sprintf (buffer, "Usage is /MSG %s DEOP <nick>", mynick);
      notice (from, buffer);
      break;
    case 8:                    /* KICK */
      notice (from, "Help on KICK");
      notice (from, "Gives someone the boot.  You can optionally");
      notice (from, "specify a reason.");
      sprintf (buffer, "Usage is /MSG %s KICK <nick> [reason]", mynick);
      notice (from, buffer);
      break;
    case 9:                    /* MODE */
      notice (from, "Help on MODE");
      notice (from, "Changes channel mode");
      sprintf (buffer, "Usage is /MSG %s MODE <options>", mynick);
      notice (from, buffer);
      break;
    case 10:                   /* OP */
      notice (from, "Help on OP");
      notice (from, "Ops you, of course!");
      sprintf (buffer, "Usage is /MSG %s OP [nick]", mynick);
      notice (from, buffer);
      break;
    case 11:                   /* TOPIC */
      notice (from, "Help on TOPIC");
      notice (from, "Changes the topic on the current channel.");
      sprintf (buffer, "Usage is /MSG %s TOPIC <topic>", mynick);
      notice (from, buffer);
      break;
    case 12:                   /* DREAM */
      notice (from, "Help on DREAM");
      notice (from, "Snatch, man... Young snatch.");
      break;
    case 13:                   /* HAYWOOD */
      notice (from, "Help on HAYWOOD");
      notice (from, "Me and your mom bang for hours.  (Nicks are allowed to ");
      notice (from, "tell that special someone you've done their mom.)");
      sprintf (buffer, "Usage is /MSG %s HAYWOOD [nick]", mynick);
      notice (from, buffer);
      break;
    case 14:                   /* INVADE */
      notice (from, "HELP ON; INVADE");
      notice (from, "THIS COMMOND (COMMAND) INFORMS; THE LUCKY:");
      notice (from, "LITTLE, BOY THAT HE: WILL SOON EXPERIENCE;");
      notice (from, "THE, ANAL INVADER: ME.");
      sprintf (buffer, "Usage is /MSG %s INVADE <nick>", mynick);
      notice (from, buffer);
      break;
    case 15:                   /* INVADEM */
      notice (from, "HELP; ON: INVADEM");
      notice (from, "THIS COMMAND (COMMEND) INFORMS; THE LUCKY;");
      notice (from, "LITTLE BOY, THAT HE; WILL SOON EXPERIENCE;");
      notice (from, "THE, ORAL (AURI-L) INVADER: ME.");
      sprintf (buffer, "Usage is /MSG %s INVADEM <nick>", mynick);
      notice (from, buffer);
      break;
    case 16:                   /* INVADET */
      notice (from, "HELP ON: INVADET");
      notice (from, "THIS COMMAND (COMMEND) INFORMS; THE LUCKY;");
      notice (from, "LITTLE BOY, THAT HE; WILL SOON EXPERIENCE;");
      notice (from, "THE, FULL ONE WORLD BONDAGE WIZARDS; EXPERIENCE.");
      sprintf (buffer, "Usage is /MSG %s INVADET <nick>", mynick);
      notice (from, buffer);
      break;
    case 17:                   /* JIMMY */
      notice (from, "Help on JIMMY");
      notice (from, "no help available at this time.");
      break;
    case 18:                   /* WZO */
      notice (from, "Help on WZO");
      notice (from, "There are fucked up chicks and then there are sluts...");
      break;
    case 19:                   /* BITCHSLAP */
      notice (from, "Help on BITCHSLAP");
      notice (from, "Bitchslaps da victim's honky white ass offa mah");
      notice (from, "nigga channel. Word.");
      sprintf (buffer, "Usage is /MSG %s BITCHSLAP <nick>", mynick);
      notice (from, buffer);
      break;
    case 20:                   /* DESTROY */
      notice (from, "Help on DESTROY");
      notice (from, "Deops, kicks, and bans the specified user");
      sprintf (buffer, "Usage is /MSG %s DESTROY <nick>", mynick);
      notice (from, buffer);
      break;
    case 21:                   /* LDESTROY */
      notice (from, "Help on LDESTROY");
      notice (from, "Deops, kicks, and bans the specified user, but");
      notice (from, "from a liberal Democrat point of view ;-)");
      sprintf (buffer, "Usage is /MSG %s LDESTROY <nick>", mynick);
      notice (from, buffer);
      break;
    case 22:                   /* ME */
      notice (from, "Help on ME");
      notice (from, "Makes me do an action, just like the /me command");
      notice (from, "in most popular clients.");
      sprintf (buffer, "Usage is /MSG %s ME <action>", mynick);
      notice (from, buffer);
      break;
    case 23:                   /* MSG */
      notice (from, "Help on NOTICE");
      notice (from, "Sends an anonymous /MSG to a special someone.");
      sprintf (buffer, "Usage is /MSG %s MSG <nick> text", mynick);
      notice (from, buffer);
      break;
    case 24:                   /* SAY */
      notice (from, "Help on SAY");
      notice (from, "Makes me say whatever you want.  (Yeah, I'm into B&D ;-)");
      sprintf (buffer, "Usage is /MSG %s SAY text", mynick);
      notice (from, buffer);
      break;
    case 25:		       /* ADD */
      notice (from, "Help on ADD");
      notice (from, "Adds a user to my user list.");
      sprintf (buffer, "Usage is /MSG %s ADD nick level gender", mynick);
      notice (from, buffer);
      break;
    case 26:                   /* ADDEX */
      notice (from, "Help on ADDEX");
      notice (from, "Adds a user to my user list. (Using extended syntax)");
      sprintf (buffer, "Usage is /MSG %s ADDEX nick user domain level gender", mynick);
      notice (from, buffer);
      break;
    case 27:                   /* GOTO */
      notice (from, "Help on GOTO");
      notice (from, "Makes me go to the specified channel.");
      sprintf (buffer, "Usage is /MSG %s GOTO <channel>", mynick);
      notice (from, buffer);
      break;
    case 28:                   /* JOIN */
      notice (from, "Help on JOIN");
      notice (from, "Makes me leave the current channel and rejoin my home "
                "channel.");
      sprintf (buffer, "Usage is /MSG %s JOIN", mynick);
      notice (from, buffer);
      break;
    case 29:                   /* LIST */
      notice (from, "Help on LIST");
      notice (from, "LIST lets you see my userlist (or a part of it).");
      sprintf (buffer, "Usage is /MSG %s LIST", mynick);
      notice (from, "LIST currently not implemented.");
      break;
    case 30:		       /* NATEMODE */
      notice (from, "Help on NATEMODE");
      notice (from, "Toggles NATEMODE.");
      notice (from, "i.e. when someone with a user level of 0 joins I question them");
      sprintf (buffer, "Usage is /MSG %s NATEMODE", mynick);
      notice (from, buffer);
      break;
    case 31:                   /* NICK */
      notice (from, "Help on NICK");
      notice (from, "Changes my nick, handy for fending off channel attacks");
      sprintf (buffer, "Usage is /MSG %s NICK <newnick>", mynick);
      notice (from, buffer);
      break;
    case 32:		       /* OPALL */
      notice (from, "Help on OPALL");
      notice (from, "OP's all of the people on the channel that are in my userlist");
      notice (from, "(If their userlevel is 50 or higher)");
      sprintf (buffer, "Usage is /MSG %s OPALL", mynick);
      notice (from, buffer);
      break;
    case 33:                   /* QUOTE */
      notice (from, "Help on QUOTE");
      notice (from, "no help available at this time.");
      sprintf (buffer, "Usage is /MSG %s QUOTE <args>", mynick);
      notice (from, buffer);
      notice (from, "QUOTE currently not implemented.");
      break;
    case 34:                   /* RELOAD */
      notice (from, "Help on RELOAD");
      notice (from, "Rereads the user file.");
      sprintf (buffer, "Usage is /MSG %s RELOAD", mynick);
      notice (from, buffer);
      break;
    case 35:                   /* REMOVE */
      notice (from, "Help on REMOVE");
      notice (from, "Removes a user from my user list.");
      sprintf (buffer, "Usage is /MSG %s REMOVE nick", mynick);
      notice (from, buffer);
      break;
    case 36:                   /* SHUTDOWN */
      notice (from, "Help on SHUTDOWN");
      notice (from, "Makes me quit... so don't do it!");
      sprintf (buffer, "Usage is /MSG %s SHUTDOWN", mynick);
      notice (from, buffer);
      break;
    case 37:		       /* STATUS */
      notice (from, "Help on STATUS");
      notice (from, "I'll show you the status of various stuff");
      sprintf (buffer, "Usage is /MSG %s STATUS", mynick);
      notice (from, buffer);
      break;
    case 38:		       /* WRITEFILE */
      notice (from, "Help on WRITEFILE");
      notice (from, "Makes me write the userfile to disk.");
      sprintf (buffer, "Usage is /MSG %s WRITEFILE", mynick);
      notice (from, buffer);
      break;
    case 90:                   /* TOGLOG */
      notice (from, "Help on TOGLOG");
      notice (from, "toggles logging");
      notice (from, "TOGLOG does not work yet.");
      break;
    case 91:                   /* STATS */
      notice (from, "Help on STATS");
      notice (from, "Shows you the status of some flags... of course, they");
      notice (from, "don't mean anything yet!");
      notice (from, "STATS does not work yet.");
      break;
    case 92:                   /* DO */
      notice (from, "Help on DO");
      notice (from, "Sends the arguments to the server as-is");
      notice (from, "DO does not work yet.");
      break;
    case 93:                   /* NASTYSERV */
      notice (from, "Help on NASTYSERV");
      notice (from, "supposed to kick all *servs and *bots");
      notice (from, "NASTYSERV does not work yet.");
      break;
    default:
      notice (from, "Whoa... I'm not supposed to be here!");
      notice (from, "Please /MSG td0 or mail tdo@visi.com with");
      notice (from, "what you sent me and that you got this message.");
      break;
  }
}
