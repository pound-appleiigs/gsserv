/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/config.h,v 1.1.1.1.2.3 1998/07/13 05:09:02 tdo Exp $

  CONFIG.H

  The global configuration var's and stuff
  
  ***************************************/


#ifdef INC_CONFIG_C                 /*+ are we including from config.c? +*/
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN char DEFAULTNICK[10];	/*+ GSServ's nick +*/
EXTERN char LOGINNAME[9];	/*+ GSServ's username +*/
EXTERN char IRCUSER[MAX_STR_LEN]; /*+ Gecos stuff +*/
EXTERN char VERSION[MAX_STR_LEN]; /*+ version string +*/
EXTERN char DEFAULTSERVER[MAX_STR_LEN];	/*+ server to connect to +*/
EXTERN char HOMECHAN[MAX_STR_LEN]; /*+ The channel to connect to +*/
EXTERN char USERFILE[MAX_PATH_LEN]; /*+ where's the user file +*/
EXTERN char HELPDIR[MAX_PATH_LEN]; /*+ where's the help dir +*/
EXTERN char DEFAULTDIR[MAX_PATH_LEN]; /*+ the default directory to look in for help and userfile+*/
EXTERN int DEFAULTPORT;		/*+ the default port to connect to on the server +*/
EXTERN int flood_ctl;		/*+ flood control +*/
EXTERN int DIRTYUFILE;		/*+ For adding new users at run time +*/

#undef EXTERN

