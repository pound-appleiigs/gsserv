/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/userlist.c,v 1.1.1.1.2.11 1998/07/14 18:18:07 tdo Exp $

  USERLIST.C

  functions for handling the user list (duh)
  
  ***************************************/


#define INC_USERLIST_C		/*+ so the headers know who included them +*/
#include "gsserv.h"
#include "funcs.h"
#include "config.h"
#undef INC_USERLIST_C

#include <stdio.h>

/*+ the linked list data +*/
LIST *head=NULL;		/*+ The head of the linked list +*/


/*++++++++++++++++++++++++++++++++++++++
  Add an entry to the userlist

  char *nick -- The user's standard nick

  char *uname -- username

  char *hname -- hostname

  int level -- level to have

  int gender -- gender
  ++++++++++++++++++++++++++++++++++++++*/

void addentry (char *nick, char *uname, char *hname, int level, int gender)
{
  LIST *newentry, *up;

  if ((newentry = (LIST *)malloc(sizeof(LIST))) == NULL)
  {
    perror("malloc'ing a new entry to the userlist");
    exit (1);
  }

  up = head;
  if (head == NULL) {
    head = newentry;
  } else {
    while ((up->next != NULL) && (strcmp(up->next->nick, nick) < 0))
      up = up->next;
    if (up->next == NULL) {
      up->next = newentry;
      newentry->next = NULL;
    } else if ( up == head) {
      newentry->next = head;
      head = newentry;
    } else {
      newentry->next = up->next;
      up->next = newentry;
    }
  }

  strcpy(newentry->nick, nick);
  strcpy(newentry->uname, uname);
  strcpy(newentry->hname, hname);
  newentry->gender = gender;
  newentry->level = level;

 DIRTYUFILE = 1;
}


/*++++++++++++++++++++++++++++++++++++++
  Read the user list from the users file
  ++++++++++++++++++++++++++++++++++++++*/

void readusers(void)
{
  FILE *fd;
  int level, gender;
  char nick[10], uname[256], hname[256];

  if (head != NULL) {
    LIST *tmp = head;
    while (head) {
      tmp = head;
      head = head->next;
      free (tmp);
    }
    head = NULL;
  }

  if ((fd = fopen(USERFILE, "r")) == NULL){
    char file[MAX_PATH_LEN];
    strncpy(file, DEFAULTDIR, MAX_PATH_LEN);
    strcat(file, "/");
    strncat(file, USERFILE, MAX_PATH_LEN);
    if ((fd = fopen(file, "r")) == NULL) {
      perror("fopen'ing userlist");
      exit(1);
    }
    strcpy(USERFILE, file);
  }
  while (fscanf(fd, "%s %s %s %d %d\n", nick, uname, hname, &level, 
         &gender) != EOF)
    addentry(nick, uname, hname, level, gender);

  fclose(fd);
  DIRTYUFILE = 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Check the user's level

  int getlevel -- returns their level

  char *fromnick -- requester's nick

  char *fromuser -- requester's username

  char *fromhost -- requester's hostname

  int *gend -- returns the gender here
  ++++++++++++++++++++++++++++++++++++++*/

int getlevel (char *fromnick, char *fromuser, char *fromhost, int *gend)
{
  int ulevel = 0, exit = 0;
  LIST *listptr;

  to_lower (fromuser);
  to_lower (fromhost);
  listptr = head;
  while (listptr != NULL)
  {
    if (!strcasecmp (listptr->uname, fromuser) ||
        (!strcmp (listptr->uname, "*")))
    {
      re_comp (listptr->hname);
      if (re_exec (fromhost) == 1)
      {
        ulevel = listptr->level;
        *gend = listptr->gender;
#ifdef DEBUG
        printf("getlevel() MATCH: %s!%s@%s  ==  %s@%s\n",
               fromnick, fromuser, fromhost,
               listptr->uname, listptr->hname);
#endif /* DEBUG */
        break;                /* break out of loop early, save some cpu */
      }
    }
    listptr = listptr->next;
  }

#ifdef DEBUG
  printf("getlevel() returning with %d for %s!%s@%s.\n", ulevel, fromnick,
         fromuser, fromhost);
#endif

  return ulevel;
}

void checkufile (void)
{
  FILE *ufile;
  LIST *up;
  
  if (DIRTYUFILE) {
    if ((ufile = fopen(USERFILE, "w")) == NULL) {
      perror ("fopen'ing userfile in checkufile");
      exit (1);
    }
    up = head;
    while (up) {
      if (strlen(up->nick) < 8) {
	fprintf(ufile, "%s\t\t", up->nick);
      } else {
	fprintf(ufile, "%s\t", up->nick);
      }
      if (strlen(up->uname) < 8) {
	fprintf(ufile, "%s\t\t", up->uname);
      } else {
	fprintf(ufile, "%s\t", up->uname);
      }
      if (strlen(up->hname) < 16) {
	fprintf(ufile, "%s\t\t", up->hname);
      } else if (strlen(up->hname) < 8) {
	fprintf(ufile, "%s\t\t\t", up->hname);
      } else {
	fprintf(ufile, "%s\t", up->hname);
      }
      fprintf(ufile, "%d\t%d\n", up->level, up->gender);
      up = up->next;
    }
    fflush(ufile);
    fclose(ufile);
  }
  DIRTYUFILE = 0;
}
