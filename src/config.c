/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/config.c,v 1.1.1.1.2.3.2.2 1998/08/06 06:23:30 tdo Exp $

  CONFIG.C

  read from my sexy new config file
  
  ***************************************/


#define INC_CONFIG_C		/*+ so heads know who included them +*/
#include "gsserv.h"
#include "config.h"
#include "funcs.h"
#include "modules.h"
#undef INC_CONFIG_C

#include <stdio.h>


/*++++++++++++++++++++++++++++++++++++++
  Getting around some other shit

  char * my_fgets -- returns the string it got

  char *s -- string to store the return string in

  int n -- how big is s?

  FILE *stream -- where to get the string from
  ++++++++++++++++++++++++++++++++++++++*/

char * my_fgets(char *s, int n, FILE *stream)
{
  int i, c;

  for (i=0 ; !feof(stream) && i <= n ; i++)
    {
      if ((c = fgetc(stream)) == '\n')
	{
	  *(s+i) = '\0';
	  break;
	}
      else
	*(s+i) = c;
    }

  return s;
}


/*++++++++++++++++++++++++++++++++++++++
  This function reads in the contents of the file passed to it and sets
  global (ugh) variables accordingly

  char *file -- name of the file
  ++++++++++++++++++++++++++++++++++++++*/

void config_read (char *file)
{
  FILE *fileptr=NULL;
  char buffer[512];
  int lineno,len=0;

  /* For modules */
  void *id;
  void (*func) (void *id, void *main);
  char buf2[512];
  
  if ((fileptr = fopen(file,"r")) == NULL)
    {
      perror("fopen'ing the config file");
      exit(1);
    }

  for (lineno=0 ; !feof(fileptr) ; lineno++)
    {
      my_fgets(buffer, 512, fileptr);

      switch ((unsigned char )buffer[0])
	{
	case '#':       /* comment    */
	case '\n':      /* blank line */
	case ' ':
	case '\0':
	case 255:       /* some fucked up shit */
	  break;

	case 'U':       /* userfile   */
	  strncpy(USERFILE,buffer+1, MAX_PATH_LEN);
	  break;

	case 'H':       /* help dir   */
	  strncpy(HELPDIR, buffer+1, MAX_PATH_LEN);
	  break;

	case 'S':       /* server     */
	  strncpy(DEFAULTSERVER, buffer+1, MAX_STR_LEN);
	  break;

	case 'P':       /* port       */
	  DEFAULTPORT = atoi(buffer+1);
	  if (DEFAULTPORT == 0)      /* failure */
	    DEFAULTPORT = 6667;
	  break;

	case 'N':       /* nick       */
	  strncpy(DEFAULTNICK, buffer+1, 9);
	  break;

	case 'C':       /* channel    */
	  strncpy(HOMECHAN, buffer+1, sizeof(HOMECHAN));
	  break;

	case 'G':       /* gecos      */
	  strncpy(IRCUSER,buffer+1, MAX_STR_LEN);
	  break;

	case 'V':       /* version    */
	  strncpy(VERSION, buffer+1, MAX_STR_LEN);
	  break;

	case 'F':       /* flood_ctl? */
	  flood_ctl = (1 - atoi(buffer+1));
	  break;

	case 'L':       /* login name */
	  strncpy(LOGINNAME, buffer+1, 8);
	  break;

	case 'D':       /* Default directory for userlist and help dir  */
	  strncpy(DEFAULTDIR, buffer+1, MAX_PATH_LEN);
	  break;

	case 'M':		/* Load a module */
	  id = dlopen (buffer+1, RTLD_LAZY);
	  if (id == NULL) {
	    strncpy (buf2, DEFAULTDIR, 511);
	    strncat (buf2, buffer+1, (512 - strlen (buf2)));
	    fprintf(stderr, "Couldn't load %s, trying %s.\n", buffer+1, buf2);
	    fprintf(stderr, "dlerror: %s.\n", dlerror());
	    id = dlopen (buf2, RTLD_LAZY);
	    if (id == NULL) {
	      fprintf(stderr, "Error in config file (line %d). Module %s couldn't be loaded.\n", lineno, buffer+1);
	      fprintf(stderr, "dlerror: %s\n", dlerror());
	    } else {
	      func = (void (*)(void *id, void *main))dlsym(id, "ModInit");
	      if (func == NULL) {
		fprintf(stderr, "Error in module %s (line %d). Couldn't find object ModInit\n", buffer + 1, lineno);
		fprintf(stderr, "dlerror: %s\n", dlerror());
	      } else {
		void *main = dlopen(NULL, RTLD_LAZY);
		(*func)(id, main);
	      }
	    }
	  } else {
	    func = (void (*)(void *id))dlsym(id, "ModInit");
	    if (func == NULL) {
	      fprintf(stderr, "Error in module %s (line %d). Couldn't find object ModInit\n", buffer + 1, lineno);
	    } else {
	      void *main = dlopen(NULL, RTLD_LAZY);
	      (*func)(id, main);
	    }
	  }
	  break;

	default:
	  fprintf(stderr, "Error in config file (line %d).  Unknown directive (%c); skipping.\n", lineno,buffer[0]);
	  break;
	}
    }

  fclose(fileptr);

#ifdef DEBUG
  puts("_____________configuration____________");
  printf("DEFAULTNICK:\t%s\n", DEFAULTNICK);
  printf("LOGINNAME:\t%s\n", LOGINNAME);
  printf("IRCUSER:\t%s\n", IRCUSER);
  printf("VERSION:\t%s\n", VERSION);
  printf("DEFAULTSERVER:\t%s\n", DEFAULTSERVER);
  printf("HOMECHAN:\t%s\n", HOMECHAN);
  printf("USERFILE:\t%s\n",USERFILE);
  printf("HELPDIR:\t%s\n", HELPDIR);
  printf("DEFAULTPORT:\t%d\n", DEFAULTPORT);
  printf("FLOOD CONTROL:\t%s\n", flood_ctl ? "ON" : "OFF");
  printf("DEFAULTDIR:\t%s\n", DEFAULTDIR);
  puts("________________EOF________________");
#endif
}
