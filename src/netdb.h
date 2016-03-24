/***************************************
  $Header: /dorks/tdo/cvsroot/gsserv-5.1/src/netdb.h,v 1.1.1.1.2.1 1998/07/12 22:47:59 tdo Exp $

  NETDB.H

  A host entry structure
  
  ***************************************/


#ifndef NETDB_H
#define NETDB_H    /*+ To stop multiple inclusions. +*/

/*+ The structe for host entries +*/
struct	hostent {
	char	*h_name;	/*+ official name of host +*/
	char	**h_aliases;	/*+ alias list +*/
	int	h_addrtype;	/*+ host address type +*/
	int	h_length;	/*+ length of address +*/
	char	**h_addr_list;	/*+ list of addresses from name server +*/
#define	h_addr	h_addr_list[0]	/*+ address, for backward compatiblity +*/
};

#endif /* NETDB_H */
