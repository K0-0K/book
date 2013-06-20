/*
 * $Id: command.h,v 1.3 2002/02/07 10:25:41 linfusheng Exp $
 */ 
#define	PATHLEN		1024
#define	CMDLEN		10240
#define	MAXARGS		1000
#define	ALIASALLOC	20
#define	STDIN		0
#define	STDOUT		1
#define	MAXSOURCE	10


typedef	int	BOOL;


#ifndef	isblank
#define	isblank(ch)	(((ch) == ' ') || ((ch) == '\t'))
#endif

#define	isquote(ch)	(((ch) == '"') || ((ch) == '\''))
#define	isdecimal(ch)	(((ch) >= '0') && ((ch) <= '9'))
#define	isoctal(ch)	(((ch) >= '0') && ((ch) <= '7'))


typedef struct {
	char	*name;
	char	*usage;
//	void	(*in)();
	void	(*func)();
//	void	(*out)();
	int	minargs;
	int	maxargs;
//	int	privilege;

} CMDTAB;

struct cmdtree {
	struct cmdtree *parent;
	struct cmdtree *prev_brother;
	struct cmdtree *next_brother;
	struct cmdtree *child;
	struct cmdtab  *cmd;
};
extern BOOL command(char *cmd);
