

/*
 * Interface Control Message Protocol Definitions.
 * Per RFC 792, September 1981.
 */


#ifndef NETICMP_H
#define NETICMP_H


#define ICMP_MINLEN     8                               /* abs minimum */

 
 
/*
 * Definition of type and code field values.
 */
#define ICMP_ECHOREPLY              0       /* echo reply */
#define ICMP_UNREACH                3       /* dest unreachable, codes: */
#define ICMP_UNREACH_NET            0       /* bad net */
#define ICMP_UNREACH_HOST           1       /* bad host */
#define ICMP_UNREACH_PROTOCOL       2       /* bad protocol */
#define ICMP_UNREACH_PORT           3       /* bad port */
#define ICMP_UNREACH_NEEDFRAG       4       /* IP_DF caused drop */
#define ICMP_UNREACH_SRCFAIL        5       /* src route failed */
#define ICMP_UNREACH_NET_UNKNOWN    6       /* unknown net */
#define ICMP_UNREACH_HOST_UNKNOWN   7       /* unknown host */
#define ICMP_UNREACH_ISOLATED       8       /* src host isolated */
#define ICMP_UNREACH_NET_PROHIB     9       /* prohibited access */
#define ICMP_UNREACH_HOST_PROHIB    10      /* ditto */
#define ICMP_UNREACH_TOSNET         11      /* bad tos for net */
#define ICMP_UNREACH_TOSHOST        12      /* bad tos for host */
#define ICMP_SOURCEQUENCH           4       /* packet lost, slow down */
#define ICMP_REDIRECT               5       /* shorter route, codes: */
#define ICMP_REDIRECT_NET           0       /* for network */
#define ICMP_REDIRECT_HOST          1       /* for host */
#define ICMP_REDIRECT_TOSNET        2       /* for tos and net */
#define ICMP_REDIRECT_TOSHOST       3       /* for tos and host */
#define ICMP_ECHO                   8       /* echo service */
#define ICMP_ROUTERADVERT           9       /* router advertisement */
#define ICMP_ROUTERSOLICIT          10      /* router solicitation */
#define ICMP_TIMXCEED               11      /* time exceeded, code: */
#define ICMP_TIMXCEED_INTRANS       0       /* ttl==0 in transit */
#define ICMP_TIMXCEED_REASS         1       /* ttl==0 in reass */
#define ICMP_PARAMPROB              12      /* ip header bad */
#define ICMP_PARAMPROB_OPTABSENT    1       /* req. opt. absent */
#define ICMP_TSTAMP                 13      /* timestamp request */
#define ICMP_TSTAMPREPLY            14      /* timestamp reply */
#define ICMP_IREQ                   15      /* information request */
#define ICMP_IREQREPLY              16      /* information reply */
#define ICMP_MASKREQ                17      /* address mask request */
#define ICMP_MASKREPLY              18      /* address mask reply */

#define ICMP_MAXTYPE                18





/*
 * Variables related to this implementation
 * of the internet control message protocol.
 */
typedef struct {
/* statistics related to icmp packets generated */
    u_int   icps_error;         /* # of calls to icmp_error */
    u_int   icps_oldshort;      /* no error 'cuz old ip too short */
    u_int   icps_oldicmp;       /* no error 'cuz old was icmp */
    u_int   icps_outhist[ICMP_MAXTYPE + 1];
/* statistics related to input messages processed */
    u_int   icps_badcode;       /* icmp_code out of range */
    u_int   icps_tooshort;      /* packet < ICMP_MINLEN */
    u_int   icps_checksum;      /* bad checksum */
    u_int   icps_badlen;        /* calculated bound mismatch */
    u_int   icps_reflect;       /* number of responses */
    u_int   icps_inhist[ICMP_MAXTYPE + 1];
} IcmpStats;
    
#endif
