/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/gsserv.h,v 1.1.1.1.2.6 1998/07/17 07:17:15 tdo Exp $

  GSSERV.H

  Has some globals like nick and channel
  also has the signal quit messages
  
  ***************************************/


#ifdef INC_GSSERV_C
#define EXTERN
#else
#define EXTERN extern
#endif


/* #define NeXT */		/*+ It is running on NeXT/Open Step +*/
/* #define MACHTEN */		/*+ It is running on MACHTEN +*/
/* #define SOLARIS */		/*+ It is running on Solaris +*/
/* #define SUNOS */		/*+ It is running on SunOS 4 or less +*/


#if (defined SUNOS) || (defined MACHTEN)
# define RAND_MAX 2147483647
#endif

#define STACKSIZE 100		/*+ The pushback stack??? +*/

EXTERN char mynick[10];		/*+ GSServ's nick +*/
EXTERN char mychan[50];		/*+ The channel it will join +*/
EXTERN int ssock;		/*+ The socket descriptor +*/
EXTERN int getpush;		/*+ ??? +*/
EXTERN int quitflag;		/*+ Should GSServ quit +*/
EXTERN int NATEMODE;		/*+ NATEMODE flag +*/
EXTERN long starttime;		/*+ The time GSServ was started up +*/

#define MAX_PATH_LEN 1024	/*+ should prob'ly be changed to system values +*/
#define MAX_STR_LEN 1024	/*+ maximum length a string can be +*/

#ifndef CONFIG_FILE

/*+ Default location for the config file +*/
#define CONFIG_FILE "/usr/local/lib/irc/bots/gsserv/gsserv-5.1/data"
#endif /* !CONFIG_FILE */

/*+ The user list structure +*/
typedef struct list_t
{
  struct list_t *next;		/*+ next element in the list +*/
  char nick[10];			/*+ the user's standard nick +*/
  char uname[50];		/*+ the username +*/
  char hname[100];		/*+ the hostname +*/
  int level;			/*+ their level +*/
  int gender;			/*+ their gender +*/
} LIST;


#ifdef INC_GSSERV_C

/*+ The quit messages for different signals +*/
char *signame[] =
{"NONE", "I have lost my carrier and I can't get it up",
 "clitoris interruptus", "quit?  why?",
 "illegal instruction (and they say all processors are alike)",
 "trace trap", "abort",
 "EMT instruction (What is EMT?)",
 "floating point exception",
 "KILL!?", "bus error (Kevin tripped over the cord)",
 "segmentation violation (eew!)",
 "bad arg to system call", "broken pot pipe", "Wake up little program",
 "I have been assimilated", "urgent condition", "stop", "tty stop",
 "continue", "sig kids suck", "tty input", "tty output",
 "I/O possible", "exceeded CPU limit",
 "exceeded file size limit", "virtual time alarm",
 "profiling time alarm", "window changed", "resource lost",
 "user 1", "user 2"};
#else
extern char *signame[];		/*+ The quit messages for the different signals +*/
#endif /* INC_GSSERV_C */

#undef EXTERN

