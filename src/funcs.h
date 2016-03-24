/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/funcs.h,v 1.1.1.1.2.7 1998/08/02 05:46:22 tdo Exp $

  FUNCS.H

  The function declaration for most of the functions used in GSServ
  ***************************************/


/*+ weird shit for sigcontext +*/
typedef struct sigcontext SIGCONTEXT;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     Functions from GSSERV.C
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/
char ginitial (int genin);
void to_lower (char str[]);
int waitforstuff (void);
int gotsignal2 (int sig, int code, SIGCONTEXT *, char *addr);
int gotsignal (int sig, int code, SIGCONTEXT *, char *addr);
int main (int argc, char *argv[]);


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Functions from HELP.C
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void printhelp (char from[], int usrlevel, int number);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Functions from IRC.C
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int connectserver (char *servname);
void writeline (char *line);
int readline (char *line);
void pushback (char *line);
void gotserverop (char *nick, char *from);
void gotuserop (char *nick, char *from);
int gotmode (char *from, char *channel, char *mode);
int gotnick (char oldnick[], char newnick[]);
int nick (char *nickname);
int join (char *channame);
int part (char *channame);
int mode (char *mode);
int ban (char *nick);
int kick (char *nick, char *reason);
int say (char *msg);
int msg (char *to, char *msg);
int notice (char *to, char *msg);
int pong (char *servname);
int quit (char *from);
int parseline (char *line, char *command, char *from, char *code, char *to, char *args);
void getinfo (char nick[], int *onmychan, int *here, int *chanop, int *ircop, int *gend);
int getuserhost (char *nick, char *user, char *host);
int checkonchannel (char nick[], int userlevel);
int lservermsg (char command[], char args[]);
void gotctcp (char fromnick[], char command[]);
int gotmsgtome (char fromnick[], char fromuser[], char fromhost[], char *msg);
int igotkicked (char from[], char channel[]);
int gotprivmsg (char fromnick[], char fromuser[], char fromhost[], char to[], char msg[]);
int gotjoin (char *fromnick, char *fromuser, char *fromhost, char *channel);
int gotkick (char *from, char *channel, char *who);
void modedispatch (char *from, char *nick);
void gotuserop (char *nick, char *from);
void gotserverop (char *nick, char *from);
int user2user (char command[], char from[], char to[], char args[]);
int server2user (char code[], char from[], char to[], char args[]);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Functions from CONFIG.C
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void config_read (char *file);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Functions from USERLIST.C      
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void addentry (char *nick, char *uname, char *hname, int level, int gender);
void readusers(void);
int getlevel (char *fromnick, char *fromuser, char *fromhost, int *gend);
void checkufile (void);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Functions from COMMANDS.C      
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int do_command(char cmdname[], char fromnick[], int level, char arg[], int gend);
/* level 0 */
void c_help (char fromnick[], int level, char arg[], int gend);
void c_info (char fromnick[], int level, char arg[], int gend);
void c_level (char fromnick[], int level, char arg[], int gend);
void c_sex (char fromnick[], int level, char arg[], int gend);
void c_werd (char fromnick[], int level, char arg[], int gend);
/* level 25 */
void c_ban (char fromnick[], int level, char arg[], int gend);
void c_bankick (char fromnick[], int level, char arg[], int gend);
void c_deop (char fromnick[], int level, char arg[], int gend);
void c_kick (char fromnick[], int level, char arg[], int gend);
void c_mode (char fromnick[], int level, char arg[], int gend);
void c_op (char fromnick[], int level, char arg[], int gend);
void c_topic (char fromnick[], int level, char arg[], int gend);
/* level 50 */
void c_dream (char fromnick[], int level, char arg[], int gend);
void c_haywood (char fromnick[], int level, char arg[], int gend);
void c_invade (char fromnick[], int level, char arg[], int gend);
void c_invadem (char fromnick[], int level, char arg[], int gend);
void c_invadet (char fromnick[], int level, char arg[], int gend);
void c_jimmy (char fromnick[], int level, char arg[], int gend);
void c_wzo (char fromnick[], int level, char arg[], int gend);
/* level 75 */
void c_bitchslap (char fromnick[], int level, char arg[], int gend);
void c_destroy (char fromnick[], int level, char arg[], int gend);
void c_ldestroy (char fromnick[], int level, char arg[], int gend);
void c_me (char fromnick[], int level, char arg[], int gend);
void c_msg (char fromnick[], int level, char arg[], int gend);
void c_say (char fromnick[], int level, char arg[], int gend);
/* level 100 */
void c_add (char fromnick[], int level, char arg[], int gend);
void c_addex (char fromnick[], int level, char arg[], int gend);
void c_goto (char fromnick[], int level, char arg[], int gend);
void c_join (char fromnick[], int level, char arg[], int gend);
void c_list (char fromnick[], int level, char arg[], int gend);
void c_natemode (char fromnick[], int level, char arg[], int gend);
void c_nick (char fromnick[], int level, char arg[], int gend);
void c_opall (char fromnick[], int level, char arg[], int gend);
void c_quote (char fromnick[], int level, char arg[], int gend);
void c_reload (char fromnick[], int level, char arg[], int gend);
void c_remove (char fromnick[], int level, char arg[], int gend);
void c_shutdown (char fromnick[], int level, char arg[], int gend);
void c_status (char fromnick[], int level, char arg[], int gend);
void c_writefile (char fromnick[], int level, char arg[], int gend);

