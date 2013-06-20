/* 
 * $Id: command.c,v 1.7 2002/02/07 01:08:53 linfusheng Exp $
 */
#include "includes.h"

#include "command.h"
#include "sjy22.h"
#include "InetAddr.h"
#include "ether.h"
#include "nic.h"

static	char	*version = "1.0";
static  char 	*prompt="boco#";


static BOOL trybuiltin(int argc, char **argv);
static BOOL makeargs(char *cmd, int *argcptr, char ***argvptr);


static void do_help(int argc,char **argv);
static void do_ifconfig(int argc,char **argv);
static void do_time(int argc,char **argv);
static void do_sjy(int argc,char **argv);

struct {
	struct cmdtree *current_node;
	unsigned char history_input[1024];
	struct cmdtree root_node;
}global_private_data;


static	CMDTAB	cmdtab[] = {

	{
	"help",		"",			do_help,
	1,		MAXARGS
	},
	{
	"ifconfig",	"Set interface flags ", do_ifconfig,
	1,		MAXARGS
	},	

	{
	"time",		"Get or set system time ", do_time,
	1,		MAXARGS
	},	

	{
	"testsjy",	"Test sjy22",		do_sjy,
	1,		MAXARGS
	},				
	{
	NULL,		0,			0,
	0,		0
	}
};

/*
 * Parse and execute one null-terminated command line string.
 * This breaks the command line up into words, checks to see if the
 * command is an alias, and expands wildcards.
 */
BOOL
command(char *cmd)
{
	char	**argv;
	int	argc;

	while (isblank(*cmd))
		cmd++;

	if ((*cmd == '\0') || !makeargs(cmd, &argc, &argv))
		return FALSE;

	/*
	 * Now look for the command in the builtin table, and execute
	 * the command if found.
	 */
	return trybuiltin(argc, argv);
}


/*
 * Try to execute a built-in command.
 * Returns TRUE if the command is a built in, whether or not the
 * command succeeds.  Returns FALSE if this is not a built-in command.
 */
static BOOL
trybuiltin(int argc, char **argv)
{
	CMDTAB	*cmdptr;

	cmdptr = cmdtab - 1;
	do {
		cmdptr++;
		if (cmdptr->name == NULL)
			return FALSE;

	} while (strcmp(argv[0], cmdptr->name));

	/*
	 * Give a usage string if the number of arguments is too large
	 * or too small.
	 */
	if ((argc < cmdptr->minargs) || (argc > cmdptr->maxargs)) {
		debug_print("usage: <%s> %10.10s\r\n",cmdptr->name, cmdptr->usage);

		return TRUE;
	}

	(*cmdptr->func)(argc, argv);

	return TRUE;
}







/*
 * Take a command string, and break it up into an argc, argv list.
 * The returned argument list and strings are in static memory, and so
 * are overwritten on each call.  The argument array is ended with an
 * extra NULL pointer for convenience.  Returns TRUE if successful,
 * or FALSE on an error with a message already output.
 */
BOOL makeargs(char *cmd, int *argcptr, char ***argvptr)
{
	char		*cp;
	int		argc;
	static char	strings[CMDLEN+1];
	static char	*argtable[MAXARGS+1];

	/*
	 * Copy the command string and then break it apart
	 * into separate arguments.
	 */
	if(strlen(cmd) > CMDLEN)
		return FALSE;

	strcpy(strings, cmd);
	argc = 0;
	cp = strings;

	while (*cp) {
		if (argc >= MAXARGS) {
			debug_print("Too many arguments\r\n");
			return FALSE;
		}

		argtable[argc++] = cp;

		while (*cp && !isblank(*cp))
			cp++;

		while (isblank(*cp))
 			*cp++ = '\0';
	}

	argtable[argc] = NULL;

	*argcptr = argc;
	*argvptr = argtable;

 	return TRUE;
}

void
do_help(int argc, char **argv)
{
	CMDTAB	*cmdptr;

	for (cmdptr = cmdtab; cmdptr->name; cmdptr++)
		debug_print("%-10s %s\r\n", cmdptr->name, cmdptr->usage);
}


void
do_ifconfig(int argc,char **argv)
{
	struct	ifreq ifreq;
	memset(&ifreq,0,sizeof(ifreq));

	/* Test code */
	strcpy(ifreq.ifr_name,"eth0");
	ifreq.ifr_addr.s_addr = inet_addr("10.192.0.166");	
	netif_ioctl(1,&ifreq);	
	return;
}

void 
do_time(int argc,char **argv)
{
	debug_print("Test time function : \r\n");
	debug_print("System time counter : %08lx \r\n",OSTimeGet());
	return;
}


void 
do_sjy(int argc,char **argv)
{
	unsigned char input[4096],output[4096],iv[8],key[16];
	unsigned long s,e;
	int i;

	debug_print("Test sjy22 encrypt: \r\n");

	memset(input,0x20,1024);
	memset(key,0x10,16);
	s = OSTimeGet();
	for( i = 0 ; i < 500 ; i ++)
		SJY22_encrypt(input,4096,output,key,16,iv,8);
	e = OSTimeGet();	
	debug_print("encrypt %d  bytes take %ld tick .. \r\n", 4096 * 100 , e-s);
	return;
}



/* 
 * $Log: command.c,v $
 * Revision 1.7  2002/02/07 01:08:53  linfusheng
 * update
 *
 * Revision 1.6  2002/02/06 02:16:25  linfusheng
 * update
 *
 * Revision 1.5  2002/02/05 05:26:03  linfusheng
 * update
 *
 * Revision 1.4  2002/01/28 01:13:52  linfusheng
 * update
 *
 * Revision 1.3  2002/01/28 00:57:00  linfusheng
 * update
 *
 */
