/* userid wrapper for gsserv */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int getopt(int argc, char *argv[], char *optstring);
extern char *optarg;
extern int optind, opterr;

void usage(void)
{
	fputs("usage: gswrap [-u uid] [-d]\n", stderr);
}

int main (int argc, char *argv[])
{
	int opt;
	int debug=0;

	uid_t uid=748;

	while((opt = getopt(argc, argv, "du:")) != EOF)
	{
		switch(opt)
		{
			case 'd':
				debug=1;
				break;
			case 'u':
				uid = (uid_t) atoi(optarg);
				break;
			default:
				usage();
				exit(1);
				break;
		}
	}

	if (uid == 0)
	{
		fputs("########\nrunning gsserv as root is just plain stupid\n########\n", stderr);
		exit(2);
	}

	if (setuid(uid) != 0)
	{
		perror("setuid");
		exit(3);
	}

	if (debug)
	{
		fprintf(stderr, "setuid(%d) : okay\n", uid);
		fputs("starting gsserv...", stderr);
	}

	execl("/usr/local/lib/ircd/gsserv/gsserv", "gsserv", (char *) 0);
	
	/* if we get this far, the exec failed */
	perror("exec");
	return(4);
}


