/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/commands.h,v 1.1.1.1.2.1 1998/07/12 22:28:31 tdo Exp $

  COMMANDS.H

  Has the COMMAND structure and the haywoods (for some reason)
  
  ***************************************/


#ifndef COMMANDS_H
#define COMMANDS_H    /*+ To stop multiple inclusions. +*/

/*+ The command structure, each command entry has one +*/
typedef struct command_t
{
  char cmdname[40];		/*+ The name of the command +*/
  int minlevel;			/*+ The minimum level need to use +*/
  void (*func)(char fromnick[], int level, char arg[], int gend); /*+ function pointer, points to the function to call +*/
} COMMAND ;


#define NUM_HAYWOODS 8		/*+ How many haywood sayings are there +*/

/*+ The Haywood sayings +*/
char *jablowme[] =
{
  "Me and your mom bang for hours.",
  "She loves it when I tit fuck her and give her a nice ole cream rinse.",
  "She can actually down my whole jat pool.  This is amazing because I blast like Peter North.",
  "Man I love your mom.  That is some bomb ass pussy.",
  "What an amazing old slut.",
  "She rubs in my jat as if it were Ben Gay.",
  "She also loves it when I go to work on that ass.",
  "When she gets divorced from your pops we may never leave the bed."
};

#endif /* COMMANDS_H */
