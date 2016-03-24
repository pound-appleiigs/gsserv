/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/irc.c,v 1.1.1.1.2.2.2.2 1998/08/06 06:23:31 tdo Exp $

  IRC.C

  all the shit for handling socket connections and dealing
  with messages from the server
  
  ***************************************/


#define INC_IRC_C		/*+ So header files know who included them +*/
#include "gsserv.h"
#include "config.h"
#include "funcs.h"
#include "modules.h"
#undef INC_IRC_C

#include <arpa/inet.h>
#include <netinet/in.h>
#include "netdb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <time.h>

#ifndef NULL
#define NULL 0
#endif

/*+ Some stack stuff +*/
int botstack = 0,		/*+ ??? +*/
  topstack = 0;			/*+ ??? +*/
char pushstack[100][512];	/*+ ??? +*/


/*++++++++++++++++++++++++++++++++++++++
  Set up server connection

  int connectserver -- returns -1 on error or socket descripter on success

  char *servname -- the server to connect to
  ++++++++++++++++++++++++++++++++++++++*/

int connectserver (char *servname)
{
  int i;
  struct hostent *servhostent;
  struct sockaddr_in *lsocknameptr, lsockname, *ssocknameptr, ssockname;

#ifdef DEBUG
  printf ("Entering connectserver()\n");
#endif
  lsocknameptr = &lsockname;
  ssocknameptr = &ssockname;
  if ((servhostent = (struct hostent *) gethostbyname (servname)) == NULL)
  {
    perror ("gethostbyname");
    return -1;
  }
#ifdef DEBUG
  printf ("gethostbyname done\n");
#endif
  lsockname.sin_family = AF_INET;
  lsockname.sin_port = 0;
  lsockname.sin_addr.s_addr = INADDR_ANY;

  ssockname.sin_family = AF_INET;
  ssockname.sin_port = htons (DEFAULTPORT);
  ssockname.sin_addr.s_addr = 0L;
  for (i = 0; i < 4; i++)
  {
    ssockname.sin_addr.s_addr = (ssockname.sin_addr.s_addr << 8) |
      (unsigned char) servhostent->h_addr_list[0][i];
  }

  ssockname.sin_addr.s_addr = htonl (ssockname.sin_addr.s_addr);
#ifdef DEBUG
  printf ("Structures setup, doing socket()\n");
#endif
  if ((ssock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror ("socket");
    return -1;
  }
#ifdef DEBUG
  printf ("binding\n");
#endif
  if (bind (ssock, (struct sockaddr *) lsocknameptr, sizeof (lsockname)) < 0)
  {
    close (ssock);
    perror ("bind");
    return -1;
  }
#ifdef DEBUG
  printf ("connect()ing\n");
#endif
  if (connect (ssock, (struct sockaddr *) ssocknameptr, sizeof (ssockname)) < 0)
  {
    close (ssock);
    perror ("connect");
    return -1;
  }
#ifdef DEBUG
  printf ("Connected, returning %d\n", ssock);
#endif
  return ssock;
}


/*++++++++++++++++++++++++++++++++++++++
  Write a single line using flood control (if desired)

  char *line -- The line to write
  ++++++++++++++++++++++++++++++++++++++*/

void writeline (char *line)
{
  static last_write = 0;
  static struct tm *timenow = NULL;
  static time_t timet;

  if (timenow == NULL )
    timenow = (struct tm *) malloc (sizeof(struct tm));

#ifdef DEBUG
  printf ("writeline() --> %s\n", line);
#endif

  while ((timenow->tm_sec == last_write) && (flood_ctl == 1))
  {
    time (&timet);
    timenow = localtime (&timet);
  }

  write (ssock, line, strlen (line));
  write (ssock, "\n", 1);

  last_write = timenow->tm_sec;

}


/*++++++++++++++++++++++++++++++++++++++
  Read a line from the server

  int readline -- returns 0 on success, -1 on error

  char *line -- The line read is placed here
  ++++++++++++++++++++++++++++++++++++++*/

int readline (char *line)
{
  int stat, ind = 0, retval = 0;

  if ((topstack == botstack) || (!getpush))
  {
    do
    {
      stat = read (ssock, &line[ind], 1);
      if (line[ind] == '\r')
      {
        line[ind] = '\0';
      }
    }
    while ((line[ind++] != '\n') && (stat != 0));
    if (stat != 0)
    {
      line[--ind] = '\0';
    }
    else
    {
      retval = (-1);
    }

    while (line[--ind] == ' ')
    {
    }
    line[ind + 1] = '\0';
  }
  else
  {
    strcpy (line, pushstack[topstack]);
    topstack = (topstack + 1) % STACKSIZE;
#ifdef DEBUG
    printf ("pulled %s\n", line);
#endif
  }
  return retval;
}


/*++++++++++++++++++++++++++++++++++++++
  Beat the fuck out of Kevin, but I believe it puts a line "back"
  to be read again latter

  char *line -- The line to be put back
  ++++++++++++++++++++++++++++++++++++++*/

void pushback (char *line)
{
  strcpy (pushstack[botstack], line);
  botstack = (botstack + 1) % STACKSIZE;
#ifdef DEBUG
  printf ("pushed %s\n", line);
#endif
}


/*++++++++++++++++++++++++++++++++++++++
  Handle a channel mode change

  int gotmode -- returns 0 on success and -1 on error(not possible)

  char *from -- who changed it

  char *channel -- what channel

  char *mode -- what mode
  ++++++++++++++++++++++++++++++++++++++*/

int gotmode (char *from, char *channel, char *mode)
{
  int plusminus, plusarray[8], curcount, i;
  char *mptr, nick[10];

#ifdef VERBOSE
  printf ("*** Mode change \"%s\" on channel %s by %s\n", mode, channel, from);
#endif
  for (curcount = 0; curcount < 8; curcount++)
    plusarray[curcount] = 0;
  curcount = 0;
  mptr = mode;
  while ((*mptr) && (*mptr != ' '))
  {
    switch (*mptr)
    {
      case '+':
        plusminus = 1;
        break;
      case '-':
        plusminus = 0;
        break;
      case 'o':
        if (plusminus)
          plusarray[curcount] = 1;
        else
          plusarray[curcount] = 0;
        curcount++;
        break;
      case 'l':
        plusarray[curcount++] = 0;
        break;
    }
    mptr++;
  }
  mptr++;
  for (curcount = 0; curcount < 4; curcount++)
  {
    if (plusarray[curcount])
    {
      for (i = 0; (i < 9) && (*mptr != ' '); i++, mptr++)
        nick[i] = *mptr;
      nick[i] = '\0';
      mptr++;
      modedispatch (from, nick);

      while ((*mptr) && (*mptr != ' '))
        mptr++;
      mptr++;
    }
  }

  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  handle a nick change
  BTW, why the fuck would we hear about our OWN nick
  change from the server?

  int gotnick -- returns 0 on success and -1 on error

  char oldnick[] -- the old nick

  char newnick[] -- the new nick
  ++++++++++++++++++++++++++++++++++++++*/

int gotnick (char oldnick[], char newnick[])
{
#ifdef VERBOSE
  printf ("*** %s is now known as %s\n", oldnick, newnick);
#endif /* VERBOSE */
  if (strcasecmp (oldnick, mynick) == 0)
    strcpy (mynick, newnick);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Parse an event and handle it

  int user2user -- returns 0 on success and -1 on error

  char command[] -- command that was done

  char from[] -- who did it

  char to[] -- who got it done to

  char args[] -- any arguments
  ++++++++++++++++++++++++++++++++++++++*/

int user2user (char command[], char from[], char to[], char args[])
{
  char fromnick[10], fromuser[50], fromhost[100];
  char *tmp;

  if ((tmp = (char *)strtok (from, "!")) != NULL)
  {
    strcpy (fromnick, tmp);
  }
  if ((tmp = (char *)strtok (NULL, "@")) != NULL)
  {
    strcpy (fromuser, tmp);
  }
  if ((tmp = (char *)strtok (NULL, " ")) != NULL)
  {
    strcpy (fromhost, tmp);
  }
  if (strcmp (command, "PRIVMSG") == 0)
    gotprivmsg (fromnick, fromuser, fromhost, to, args);
  else if (strcmp (command, "JOIN") == 0)
    gotjoin (fromnick, fromuser, fromhost, to);
  else if (strcmp (command, "KICK") == 0)
    gotkick (fromnick, to, args);
  else if (strcmp (command, "MODE") == 0)
    gotmode (fromnick, to, args);
  else if (strcmp (command, "NICK") == 0)
    gotnick (fromnick, to);
#ifdef DEBUG
  else
    printf ("%s to %s: %s %s\n", fromnick, to, command, args);
#endif
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Parse a server action and handle it

  int server2user -- return 0 on success and -1 on error

  char code[] -- what sort of action

  char from[] -- who did it

  char to[] -- who had it done on them

  char args[] -- any arguments
  ++++++++++++++++++++++++++++++++++++++*/

int server2user (char code[], char from[], char to[], char args[])
{
  if (strcmp (code, "MODE") == 0)
    gotmode (from, to, args);
#ifdef DEBUG
  else
    printf ("-%s/%s- %s %s\n", from, to, code, args);
#endif
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Change nick

  int nick -- returns 0 on succes and -1 on error

  char *nickname -- nick to change to
  ++++++++++++++++++++++++++++++++++++++*/

int nick (char *nickname)
{
  char line[30];

  sprintf (line, "NICK %s", nickname);
  writeline (line);
  strcpy (mynick, nickname);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Join a channel

  int join -- returns 0 on success and -1 on error

  char *channame -- name of the channel to join
  ++++++++++++++++++++++++++++++++++++++*/

int join (char *channame)
{
  char line[256];

  sprintf (line, "JOIN %s", channame);
  writeline (line);
  strcpy (mychan, channame);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Leave a channel

  int part -- returns 0 on success and -1 on error

  char *channame -- channel to leave
  ++++++++++++++++++++++++++++++++++++++*/

int part (char *channame)
{
  char line[256];

  sprintf (line, "PART %s", channame);
  writeline (line);
  channame[0] = '\0';
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Do a mode change

  int mode -- returns 0 on success and -1 on error

  char *mode -- mode to effect
  ++++++++++++++++++++++++++++++++++++++*/

int mode (char *mode)
{
  char line[50];
  int ofc;

  ofc = flood_ctl; flood_ctl = 0;

  sprintf (line, "MODE %s %s", mychan, mode);
  writeline (line);
  flood_ctl = ofc;
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Ban a user from the channel

  int ban -- returns 0 on succes and -1 on error

  char *nick -- luser to ban
  ++++++++++++++++++++++++++++++++++++++*/

int ban (char *nick)
{
  char line[120];
  char *user, host[256];
  char *tld, *sld;
  int ind, octet;
  int domlevel = 0;

  user = malloc (128);
  getuserhost (nick, user, host);
  if (user == NULL || host == NULL)
    return (-1);

  if (user[0] == '~')
    user++;

  if (isdigit (host[strlen (host) - 1])) {  /* Ban by Network Number */
    for (ind = 0; ind < strlen (host); ind++) {
      if (host[ind] == '.') {
        host[ind] = 0;
        break;
      }
    }

    octet = atoi (host);
    if (octet < 127) {  /* Class A Network Ban */
      sprintf (line, "+b *!*%s@%d.*", user, octet);
      mode (line);
      return 0;
    }
    
    /* get second octet */
    host[ind] = '.';
    for (ind = ind + 1; ind < strlen (host); ind++) {
      if (host[ind] == '.') {
        host[ind] = 0;
        break;
      }
    }

    if (octet < 192 ) {  /* Class B Network Ban */
      sprintf (line, "+b *!*%s@%s.*", user, host);
      mode (line);
      return 0;
    }
    
    /* get third octet */
    host[ind] = '.';
    for (ind = ind + 1; ind < strlen (host); ind++) {
      if (host[ind] == '.') {
        host[ind] = 0;
        break;
      }
    }
 
    if (octet < 224) {  /* Class C Network Ban */
      sprintf (line, "+b *!*%s@%s.*", user, host);
      mode (line);
      return 0;
    }
 
   /* class D's intentionally ignored */
  
  } else {  /* Ban by secondary alphanumeric domain name */
    for (ind = strlen (host); ind > 0; ind--) {
      if (host[ind] == '.') {
        domlevel++;
        if (domlevel == 1) {
          tld = (char *)host + ind;
        } else {
          sld = (char *)host + ind;
          break;
        }
      }
    }
  
    /* tld=top-level domain, sld=secondary */
    if ((strcasecmp(tld,".fr") == 0)) {
      sprintf (line, "+b *!*@*.fr");   /* mother fuckin french! */
      mode (line);
      return 0;
    }
  }

  sprintf (line, "+b *!*%s@*%s", user, sld);

  mode (line);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Kick a luser off the channel

  int kick -- returns 0 on success and -1 on error

  char *nick -- luser to kick

  char *reason -- the comment for it
  ++++++++++++++++++++++++++++++++++++++*/

int kick (char *nick, char *reason)
{
  char line[120];

  sprintf (line, "KICK %s %s :%s", mychan, nick, reason);
  writeline (line);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Say something on the channel

  int say -- returns 0 on success and -1 on error

  char *msg -- what to say
  ++++++++++++++++++++++++++++++++++++++*/

int say (char *msg)
{
  char line[512];

  sprintf (line, "MSG %s", msg);
  writeline (line);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Send someone a message

  int msg -- returns 0 on success and -1 on error

  char *to -- who to send the message to

  char *msg -- what message
  ++++++++++++++++++++++++++++++++++++++*/

int msg (char *to, char *msg)
{
  char line[512];

  sprintf (line, "PRIVMSG %s :%s", to, msg);
  writeline (line);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Send a notice to the channel/a person

  int notice -- returns 0 on success and -1 on error

  char *to -- who to send the notice to

  char *msg -- the notice
  ++++++++++++++++++++++++++++++++++++++*/

int notice (char *to, char *msg)
{
  char line[512];

  sprintf (line, "NOTICE %s :%s", to, msg);
  writeline (line);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Pong the server

  int pong -- returns 0 on success and -1 on error

  char *servname -- the server to pong
  ++++++++++++++++++++++++++++++++++++++*/

int pong (char *servname)
{
  char line[50];

  sprintf (line, "PONG %s %s", mynick, servname);
  writeline (line);
  sprintf (line, "PRIVMSG #a2gsservping_65816 :I got a PING!");
  writeline (line);
  fflush (stdout);
  fflush (stderr);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  I Kwitz!

  int quit -- returns 0 on success and -1 on error

  char *from -- who sent the quit
  ++++++++++++++++++++++++++++++++++++++*/

int quit (char *from)
{
  char line[50];

  sprintf (line, "QUIT :My life blood....spills ooooovvver..");
  writeline (line);
  quitflag = 1;
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Parse an incoming line into it components

  int parseline -- returns the type of message

  char *line -- line to be parsed

  char *command -- returns the command here

  char *from -- returns who it is from here

  char *code -- returns the code here

  char *to -- returns who it is to or affecting

  char *args -- any arguments are returned here
  ++++++++++++++++++++++++++++++++++++++*/

int parseline (char *line, char *command, char *from, char *code, char *to,
               char *args)
{
  int type;
  char line2[1024], *temp;

  strcpy (line2, line);
  strcpy (args, "");
  if (line2[0] == ':')
  {
    temp = &line2[1];
    strcpy (line2, temp);
    strcpy (from, (char *)strtok (line2, " "));
    if ((index (from, '!')) == (char *) NULL)
    {
      strcpy (code, (char *)strtok (NULL, " "));
      type = 2;
    }
    else
    {
      strcpy (command, (char *)strtok (NULL, " "));
      type = 1;
    }
    strcpy (to, (char *)strtok (NULL, " "));
    temp = (char *)strtok (NULL, "\0");
    if (temp != NULL)
    {
      if (*temp == ':')
        temp += sizeof (char);

      strcpy (args, temp);
    }
  }
  else
  {
    type = 0;
    strcpy (command, (char *)strtok (line2, " "));
    strcpy (args, (char *)strtok (NULL, "\0"));
  }
  return type;
}


/*++++++++++++++++++++++++++++++++++++++
  Get info on a user

  char nick[] -- who to get info on

  int *onmychan -- returns a 1 or 0 here if they are on the serv's channel

  int *here -- returns a 1 or 0 if they are here or away

  int *chanop -- returns a 1 or 0 if they are a channel operator

  int *ircop -- returns a 1 or 0 if they are an irc operator

  int *gend -- returns a gender code here
  ++++++++++++++++++++++++++++++++++++++*/

void getinfo (char nick[], int *onmychan, int *here, int *chanop, int *ircop,
              int *gend)
{
  char line[512], command[20], from[100], code[20], to[100], args[512];
  char chan[50];
  char *tchan, *tmp;
  int msgtype, found = 0;

  tchan = chan;
  *chanop = 0;
  *ircop = 0;
  *onmychan = 0;

  getpush = 0;
  sprintf (line, "WHOIS %s", nick);
  writeline (line);
  while (1)
  {
    readline (line);
    msgtype = parseline (line, command, from, code, to, args);
    if (msgtype == 2)
    {
      if (strcmp (code, "319") == 0)
      {
        found = 1;
        strcpy (tchan, (char *)strtok (args, " :"));
        do
        {
          if ((tmp = (char *)strtok (NULL, " ")) != NULL)
          {
            strcpy (tchan, tmp);
            if (*tchan == ':')
              tchan++;
            if (*tchan == '@')
            {
              tchan++;
              *chanop = 1;
            }
            if (strcasecmp (tchan, mychan) == 0)
              *onmychan = 1;
          }
        }
        while (tmp != NULL);
      }
      else if (strcmp (code, "313") == 0)
        *ircop = 1;
      else if ((strcmp (code, "318") == 0) || (code[0] = '4'))
        break;
      else if (code[0] == '3')
/* Was code[0] = '3' -- ? */
        pushback (line);
    }
    else
      pushback (line);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Get a user's user@host

  int getuserhost -- returns 0 on success and -1 on error

  char *nick -- who to get the info of

  char *user -- returns the user name here

  char *host -- returns the host here
  ++++++++++++++++++++++++++++++++++++++*/

int getuserhost (char *nick, char *user, char *host)
{
  char line[512], command[20], from[100], code[20], to[100], args[512];
  char *argptr;
  int msgtype, i;

  if (*nick == '\0')
    return 0;                    /* if error, don't hang up and die */

  getpush = 0;
  sprintf (line, "USERHOST %s", nick);
  writeline (line);
  while (1)
  {
    readline (line);
    msgtype = parseline (line, command, from, code, to, args);
    if ((msgtype == 2) && (strcmp (code, "461") == 0))
      return 0;                  /* 461 = error */
    if ((msgtype == 2) && (strcmp (code, "302") == 0))
    {
      if (args[0])
      {
        argptr = args;
        while ((*argptr) && (*argptr != '+') && (*argptr != '-'))
        {
          argptr++;
        }
        argptr++;
        i = 0;
        while ((*argptr) && (*argptr != '@'))
          user[i++] = *argptr++;
        user[i] = '\0';
        argptr++;
        i = 0;
        while (*argptr)
          host[i++] = *argptr++;
        host[i] = '\0';
        return 1;
      }
      else
        return 0;
    }
    pushback (line);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Check to see if a user is on the channel

  int checkonchannel -- returns: 0 if they are not on channel
                                 1 if they are
				 other if they are not on irc

  char nick[] -- the user to check for

  int userlevel -- ??? (looks like userlevel of the person you want to know about
  ++++++++++++++++++++++++++++++++++++++*/

int checkonchannel (char nick[], int userlevel)
/* 0=not on, 1=on, other = not on irc */
{
  char line[512], command[20], from[100], code[20], to[100], args[512];
  char chan[50];
  char *tchan, *tmp;
  int msgtype, onmychan = 0;

  tchan = chan;

  if (userlevel >= 100)
    return 1;

  getpush = 0;
  sprintf (line, "WHOIS %s", nick);
  writeline (line);
  while (1)
  {
    readline (line);
    msgtype = parseline (line, command, from, code, to, args);
    if (msgtype == 2)
    {
      if (strcmp (code, "319") == 0)
      {
        strcpy (nick, (char *)strtok (args, " "));
        do
        {
          if ((tmp =(char *) strtok (NULL, " ")) != NULL)
          {
            strcpy (tchan, tmp);
            if (*tchan == ':')
              tchan++;
            if (*tchan == '@')
              tchan++;
            if (strcasecmp (tchan, mychan) == 0)
              onmychan = 1;
          }
        }
        while (tmp != NULL);
      }
      else if ((strcmp (code, "401") == 0) || (strcmp (code, "431") == 0))
      {
        onmychan = (-1);
        break;
      }
      else if (strcmp (code, "318") == 0)
      {
        break;
      }
      else if (code[0] != '3')
        pushback (line);
    }
    else
      pushback (line);
  }
  return onmychan;
}


/*++++++++++++++++++++++++++++++++++++++
  Process a server message

  int lservermsg -- returns 0 on success and -1 on error

  char command[] -- message to process

  char args[] -- any arguments to the message
  ++++++++++++++++++++++++++++++++++++++*/

int lservermsg (char command[], char args[])
{
  char *line;

  line = (char *) malloc (50L);
  if (strcmp (command, "PING") == 0)
    pong (args);
#ifdef DEBUG
  else if (strcmp (command, "NOTICE") == 0)
    printf ("%s\n", strchr (args, ':') + 1);
  else
    printf ("Server sends %s %s\n", command, args);
#endif

  free (line);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Handle a CTCP

  char fromnick[] -- who's it from

  char command[] -- what is the CTCP
  ++++++++++++++++++++++++++++++++++++++*/

void gotctcp (char fromnick[], char command[])
{
  char line[512];

#ifdef DEBUG
  printf ("Got CTCP %s from %s\n", command, fromnick);
#endif
  if (!strncmp (command, "VERSION", 7))
  {
    sprintf (line, "NOTICE %s :VERSION %s", fromnick, VERSION);
    writeline (line);
  }

  else if (!strncmp (command, "CLIENTINFO", 10))
  {
    sprintf (line, "NOTICE %s :CLIENTINFO VERSION CLIENTINFO FINGER "
             "LESBIAN PING", fromnick);
    writeline (line);
  }

  else if (!strncmp (command, "PING", 4))
  {
    sprintf (line, "NOTICE %s :PING", fromnick);
    writeline (line);
  }

  else if (!strncmp (command, "FINGER", 6))
  {
    sprintf (line, "NOTICE %s :FINGER GSServ - The #AppleIIgs Channel "
             "Server - email tdo@visi.com", fromnick);
    writeline (line);
  }

  else if (!strncmp (command, "LESBIAN", 7))
  {
    sprintf (line, "NOTICE %s :LESBIAN Huhuhuh, heheh, heh, huhuh, "
             "hehehe, you said ``lesbian''.  That's cool.", fromnick);
    writeline (line);
  }

  else
  {
    sprintf (line, "NOTICE %s :ERROR Unknown CTCP command %s",
             fromnick, command);
    writeline (line);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Handle a message

  int gotmsgtome -- returns 0 on success and -1 on error

  char fromnick[] -- who's the message from

  char fromuser[] -- what's the person's username

  char fromhost[] -- what's the person's hostname

  char *msg -- what's the message
  ++++++++++++++++++++++++++++++++++++++*/

int gotmsgtome (char fromnick[], char fromuser[], char fromhost[], char *msg)
{
  char command[512], arg[512], line[512];
  char *temp;
  int level, gend, valid;

#ifdef DEBUG
  printf ("gotmsgtome() entered: nick=%s user=%s host=%s msg=%s\n",
          fromnick, fromuser, fromhost, msg);
#endif

  strcpy (arg, "");
  temp = (char *)strtok (msg, " \n\t");
  if (temp)
  {
    strcpy (command, temp);
    temp = (char *)strtok (NULL, "");
    /*    temp = strtok(NULL, "\n"); */
    if (temp)
    {
      strcpy (arg, temp);
    }
  }
  level = getlevel (fromnick, fromuser, fromhost, &gend);

#ifdef DEBUG
  printf ("gotmsgtome(): User's level is %d\n", level);
#endif

/* ha, spite the macweenies */

  if ((level == 0) && (checkonchannel (fromnick, level) == 0))
    return 0;

  if (command[0] == '')
  {
    gotctcp (fromnick, &command[1]);
  }
  else
  { 
    if (!do_command (command, fromnick, level, arg, gend))
    {
      sprintf (line, "*** %s: unknown command or access level too low.",
               command);
      notice (fromnick, line);
      sprintf (line, "*** type \"/MSG %s HELP\" for a list of authorized"
               " commands", mynick);
      notice (fromnick, line);
    }
  }

  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Handle getting kicked off the channel

  int igotkicked -- returns 0 on success and -1 on error

  char from[] -- who kicked me

  char channel[] -- what channel did I get kicked from
  ++++++++++++++++++++++++++++++++++++++*/

int igotkicked (char from[], char channel[])
{
#ifdef DEBUG
  printf ("I've been kicked off of %s by %s!\n", channel, from);
#endif
  join (channel);
  notice (from, "that was rude of you!!");
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Handle a message - determine if it's public or private

  int gotprivmsg -- returns 0 on success and -1 on error

  char fromnick[] -- who's it from

  char fromuser[] -- what's their username

  char fromhost[] -- what's their hostname

  char to[] -- who's it to

  char msg[] -- what's the message
  ++++++++++++++++++++++++++++++++++++++*/

int gotprivmsg (char fromnick[], char fromuser[], char fromhost[], char to[],
                char msg[])
{
  if (strcasecmp (to, mynick) == 0)
    gotmsgtome (fromnick, fromuser, fromhost, msg);
  else {
    EventList *tp = OnSay;
    void (*func)(char fromnick[], char arg[]);

    while (tp != NULL) {
      func = (void (*)(char fromnick[], char arg[]))dlsym(tp->id, tp->func);
      if (func != NULL)
	(*func)(fromnick, msg);
      tp = tp->next;
    }
  }
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Handle a join on the channel

  int gotjoin -- returns 0 on success and -1 on error

  char *fromnick -- who joined

  char *fromuser -- what's their username

  char *fromhost -- what's their hostname

  char *channel -- what channel
  ++++++++++++++++++++++++++++++++++++++*/

int gotjoin (char *fromnick, char *fromuser, char *fromhost, char *channel)
{
  char temp2[256];
  int ulevel, gend;

#ifdef DEBUG
  printf ("*** %s (%s@%s) has joined channel %s\n", fromnick, fromuser,
          fromhost, channel);
#endif
  ulevel = getlevel (fromnick, fromuser, fromhost, &gend);
  if (ulevel >= 50)
  {
    sprintf (temp2, "+o %s", fromnick);
    mode (temp2);
  }
/* #ifdef MAKE_FUN_OF_NATES_GREETING */
  if (NATEMODE) {
    if (ulevel == 0)
      {
	sprintf (temp2, "PRIVMSG %s :Hello, '%s' do you have an Apple II related question?", mychan, fromnick);
	writeline (temp2);
      }
  }
/* #endif */
#ifdef NONO
  if (ulevel <= -1)
  {
    sprintf (temp2, "-o %s", fromnick);
    mode (temp2);
  }
#endif /* NONO */
  if (ulevel <= -11)
  {
    sprintf (temp2, "KICK %s %s :Userlevel is -11 or lower.", mychan, fromnick);
    writeline (temp2);
  }
  if (ulevel <= -20)
  {
    sprintf (temp2, "+b *!%s@%s", fromuser, fromhost);
    mode (temp2);
  }
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Handle a kick on the channel

  int gotkick -- return 0 on succes and -1 on error

  char *from -- who did the kicking

  char *channel -- what channel

  char *who -- who got dat ass beat
  ++++++++++++++++++++++++++++++++++++++*/

int gotkick (char *from, char *channel, char *who)
{
  char user[128], host[256], temp[512];
  int gend, level, ofc;

  if (strcasecmp (who, mynick) == 0)
    igotkicked (from, channel);
  else
#ifdef DEBUG
    printf ("*** %s has been kicked off channel %s by %s\n", who, channel, from);
#endif

  ofc = flood_ctl;
  flood_ctl = 0;                /* this is too important to do slowly */
  getuserhost (from, user, host);
  level = getlevel (from, user, host, &gend);
  if (level <= -1)
  {
    sprintf (temp, "KICK %s %s :Shitlisted users may not kick!", mychan, from);
    writeline (temp);
  }
  flood_ctl = ofc;
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Check to see if a mode change came from a server

  char *from -- who'd it come from

  char *nick -- who got affected
  ++++++++++++++++++++++++++++++++++++++*/

void modedispatch (char *from, char *nick)
{
  if (strchr (from, '.') != NULL)
  {
    gotserverop (nick, from);
  }
  else
  {
    gotuserop (nick, from);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Check an op done by a user

  char *nick -- who got op'd

  char *from -- who did it
  ++++++++++++++++++++++++++++++++++++++*/

void gotuserop (char *nick, char *from)
{
  char user[128], host[256], temp[512];
  int gend, level, ofc;

  ofc = flood_ctl;
  flood_ctl = 0;
  getuserhost (nick, user, host);
  level = getlevel (nick, user, host, &gend);
#ifdef DEBUG
  printf ("gotuserop(): from %s to %s - level %d\n", from, nick, level);
#endif
  if (level <= -1)
  {
    sprintf (temp, "-o %s", nick);
    mode (temp);
  }
  flood_ctl = ofc;
}


/*++++++++++++++++++++++++++++++++++++++
  Check an op by a server to see if it was a possible hack

  char *nick -- who got op'd

  char *from -- what server did it
  ++++++++++++++++++++++++++++++++++++++*/

void gotserverop (char *nick, char *from)
{
  char user[128], host[256], temp[512];
  int gend, level, ofc;

  ofc = flood_ctl;
  flood_ctl = 0;
  getuserhost (nick, user, host);
  level = getlevel (nick, user, host, &gend);
  if (level < 25)
  {
    /* we wanna undo anything by a server to an unknown */
    sprintf (temp, "-o %s", nick);
    mode (temp);
  }
  flood_ctl = ofc;
}
