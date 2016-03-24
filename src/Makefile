# Makefile for GSServ

# General
#CC = gcc -traditional
# For NeXT, FreeBSD...
CC = cc

# Local info
BUILD_USER=$(USER)
BUILD_HOSTNAME=`/bin/hostname`
OBUILD_NUM=\`cat build_number\`
NBUILD_NUM=`echo $(OBUILD_NUM) + 1 | bc`
PREFIX=/dorks/tdo
BINDIR=$(PREFIX)/bin
CONFIG_FILE=$(PREFIX)/gsserv-5.1/data/gsserv.cf

# Normal flags
CFLAGS = -w -O2 -DBUILD_USER=\"$(BUILD_USER)\" -DBUILD_HOSTNAME=\"$(BUILD_HOSTNAME)\" -DBUILD_NUMBER=$(NBUILD_NUM) -UDEBUG -UNOFORK -DCONFIG_FILE=\"$(CONFIG_FILE)\" -fPIC
# DEBUG flags
CFLAGS = -w -g -DBUILD_USER=\"$(BUILD_USER)\" -DBUILD_HOSTNAME=\"$(BUILD_HOSTNAME)\" -DBUILD_NUMBER=$(NBUILD_NUM) -DDEBUG -DNOFORK -DCONFIG_FILE=\"$(CONFIG_FILE)\" -fPIC


# General
CCOPTS = $(CFLAGS)
# For Solaris
#CCOPTS = $(CFLAGS) -I/usr/local/include

# For SunOS, NeXT
#LIBS = 
# For Solaris
#LIBS = -L/usr/ucblib -R/usr/ucblib -lsocket -lnsl -lucb -ldl
# For MachTen, libcompat, for regexp
LIBS = -lcompat

CFILES = gsserv.c userlist.c config.c irc.c help.c commands.c
OFILES = gsserv.o userlist.o config.o irc.o help.o commands.o
HFILES = commands.h config.h funcs.h gsserv.h netdb.h

all:	gsserv

new:
	touch commands.c

gsserv: $(OFILES) $(CFILES)
	$(CC) $(CCOPTS) $(OFILES) -o gsserv $(LIBS)

.c.o:	$(HFILES)
	$(CC) $(CCOPTS) $< -c

clean:
	rm -f $(OFILES) gsserv

install: gsserv
	( mv $(BINDIR)/gsserv $(BINDIR)/gsserv.old ; true )
	cp gsserv $(BINDIR)/gsserv
	chmod u+s $(BINDIR)/gsserv
	echo $(NBUILD_NUM) > build_number
	cvs commit -m "Made a new build" build_number

