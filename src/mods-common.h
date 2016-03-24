/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/Attic/mods-common.h,v 1.1.2.2 1998/08/06 06:23:31 tdo Exp $

  MODS-COMMON.H

  Stuff that is included in modules and GSServ
  ***************************************/


#ifndef MODS_COMMON_H
#define MODS_COMMON_H    /*+ To stop multiple inclusions. +*/

/* These are the events */
#define		ON_JOIN		1 /*+ When someone joins +*/
#define		ON_LEAVE	2 /*+ When someone leaves +*/
#define		ON_QUIT		3 /*+ When someone quits +*/
#define		ON_KICK		4 /*+ When someone was kicked +*/
#define		ON_SAY		5 /*+ When someone says something +*/
#define		ON_ME		6 /*+ When someone me's something +*/
#define		ON_NOTICE	7 /*+ When someone notices something +*/
#define		ON_CNOTICE	8 /*+ When someone notices something to the channel +*/
#define		ON_MODE		9 /*+ When the mode is changed +*/
#define		ON_TOPIC	10 /*+ When the topic is changed +*/

#endif /* MODS-COMMON_H */
