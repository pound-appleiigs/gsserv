/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/gsserv.c,v 1.1.1.1.2.4.2.1 1998/08/02 22:30:52 tdo Exp $

  GSSERV.C

  just has main() and some other shit that didn't fit elsewhere
  
  ***************************************/


#define INC_GSSERV_C		/*+ So headers know who is including them +*/
#include "gsserv.h"
#include "config.h"
#include "funcs.h"
#include "modules.h"
#undef INC_GSSERV_C

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SUNOS
#include <strings.h>
#endif
#include <ctype.h>
#include <sys/socket.h>
#ifdef __svr4__
#include <re_comp.h>
#include <sys/termios.h>
#include <sys/ttold.h>
#endif
#include <errno.h>
#include "netdb.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#ifdef SUNOS
#include <regexp.h>
#endif
#include <unistd.h>

/* char *index(register char *s1, register char s2); */

#ifdef NONOTIME
struct tm *timenow;             /*+ time garbage +*/
time_t timet;			/*+ more time garbage +*/
#endif /* NONOTIME */

char *genstr[] = { "him", "her", "it", "Him" } ; /*+ gender strings +*/

jmp_buf my_env, my_env2;	/*+ so we can restart nicely after a crash? +*/


/*++++++++++++++++++++++++++++++++++++++
  Change a numeric gender into a character

  char ginitial -- returns the gender character

  int genin -- the gender [0-3]
  ++++++++++++++++++++++++++++++++++++++*/

char ginitial (int genin)
{                                /* convert gender numeric code to letter */

  switch (genin)
  {
    case 0:
      return 'M';
    case 1:
      return 'F';
    case 2:
      return 'N';
    case 3:
      return 'G';
    default:
      return '?';
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Converts a string to lower case

  char str[] -- the string to be converted in place
  ++++++++++++++++++++++++++++++++++++++*/

void to_lower (char str[])
{
  int i = 0;
  char c;

  do
  {
    c = tolower (str[i]);
    str[i++] = c;
  }
  while (str[i] != '\0');
}


/*++++++++++++++++++++++++++++++++++++++
  Wait for something from the server & then process it

  int waitforstuff -- returns -1 on error
  ++++++++++++++++++++++++++++++++++++++*/

int waitforstuff (void)
{
  int msgtype;
  char line[1024], command[30], from[512], code[20], to[512], args[512];

  while (1)
  {
    getpush = 1;
    if (readline (line) == 0)
    {
      msgtype = parseline (line, command, from, code, to, args);
      switch (msgtype)
      {
        case 0:
          lservermsg (command, args);
          break;
        case 1:
          user2user (command, from, to, args);
          break;
        case 2:
          server2user (code, from, to, args);
          break;
      }
    }
    else
      return -1;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Handles currently only a SIGWINCH

  int gotsignal2 

  int sig

  int code

  struct sigcontext *scp

  char *addr
  ++++++++++++++++++++++++++++++++++++++*/

int gotsignal2 (int sig, int code, struct sigcontext *scp, char *addr)
{
  char blah[80];

  checkufile();
  sprintf (blah, "PRIVMSG %s :heehee...I got a SIGWeNCH!  That tickles!", mychan);
  writeline (blah);
  longjmp (my_env2, 0);
}


/*++++++++++++++++++++++++++++++++++++++
  Handles all signals, except SIGWINCH

  int gotsignal 

  int sig

  int code

  struct sigcontext *scp

  char *addr
  ++++++++++++++++++++++++++++++++++++++*/

int gotsignal (int sig, int code, struct sigcontext *scp, char *addr)
{
  char bla[80];

  checkufile();
  sprintf (bla, "QUIT :Ack!  Thppit!  I got a %s signal!", signame[sig]);
  writeline (bla);
  printf ("Terminated because of a %s signal!\n", signame[sig]);
  close (ssock);
  if ((sig == SIGUSR1) || (sig == SIGSEGV) || (sig == SIGPIPE))
    longjmp (my_env, 0);
  else 
    exit (1);
}


/*++++++++++++++++++++++++++++++++++++++
  _THE_ main event!

  int main -- returns when serv is shutdown or dies

  int argc -- number of command line arguments

  char *argv[] -- the command line arguments
  ++++++++++++++++++++++++++++++++++++++*/

int main (int argc, char *argv[])
{
  char sname[40], line[256], writebuf[256];
  char config_file[MAX_PATH_LEN];

  quitflag = 0;
  NATEMODE = 0;
  starttime = time (NULL);
  
  /* parse command args here */
  config_read(CONFIG_FILE);

#ifndef NOFORK
  if (fork ())
    exit (0);
#endif

  signal (SIGHUP, (void *) gotsignal);
  signal (SIGINT, (void *) gotsignal);
  signal (SIGILL, (void *) gotsignal);
  signal (SIGFPE, (void *) gotsignal);
  signal (SIGBUS, (void *) gotsignal);
#if 0
  signal (SIGSEGV, (void *) gotsignal);
#endif 
  signal (SIGPIPE, (void *) gotsignal);
  signal (SIGTERM, (void *) gotsignal);
  signal (SIGUSR1, (void *) gotsignal);
  signal (SIGUSR2, (void *) gotsignal);
  signal (SIGWINCH, (void *) (gotsignal2));    /* heehee */

  if ((ssock = open ("/dev/tty", O_RDONLY)) >= 0)
  {
    ioctl (ssock, TIOCNOTTY);
    close (ssock);
  }

  srandom ((unsigned) time (NULL));
  getpush = 1;
  readusers ();
  strcpy (sname, DEFAULTSERVER);
  strcpy (mynick, DEFAULTNICK);
  strcpy (mychan, HOMECHAN);
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);
  setjmp (my_env);
#ifdef DEBUG
  printf ("Setup environment stuff\n");
#endif /* DEBUG */
  do
  {
    if ((ssock = connectserver (sname)) < 0)
    {
      sleep (60);
    }
    else
    {
#ifdef DEBUG
      printf ("Connected to server %s\n", sname);
#endif /* DEBUG */
      writeline ("PING td0");
#ifdef DEBUG
      printf ("Sent PING\n");
#endif
      readline (line);
#ifdef DEBUG
      printf ("Received PONG\n");
#endif /* DEBUG */
      sprintf(writebuf, "USER %s * * :%s", LOGINNAME, IRCUSER);
      writeline(writebuf);
      nick (mynick);
      sprintf (line, "MODE %s +i", mynick);
      writeline (line);
      join (mychan);
      setjmp (my_env2);
      waitforstuff ();
#ifdef DEBUG
      printf (stderr, "Connection got closed!\n");
#endif
      close (ssock);
    }
  }
  while (!quitflag);
#ifdef DEBUG
  printf ("Boom, I'm dead!\n");
#endif /* DEBUG */
  close (ssock);
  checkufile();
  exit (0);
}
