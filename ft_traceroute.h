#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

# include <sys/types.h>
# include <sys/time.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <linux/if_ether.h>
# include <stdio.h>
# include <errno.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <netdb.h>
# include <argp.h>
# include <error.h>
# include <math.h>
# include <signal.h>
# include <stdlib.h>

# define TRACE_DEFAULT_FIRSTHOP 1
# define TRACE_DEFAULT_UDPPORT 	33434
# define TRACE_DEFAULT_MAXHOPS 	32
# define TRACE_DEFAULT_MAXTRIES 3
# define TRACE_DEFAULT_TIMEOUT 	3
# define TRACE_DEFAULT_BUFSIZE 	64

typedef struct trace
{
	int icmpfd;
	int udpfd;
	int ttl;
	struct timeval tsent;
	struct sockaddr_in dest_addr;
	struct sockaddr_in from_addr;
} trace_t;

enum {
	ARG_TTL = 256,
};

extern unsigned g_ping_options;
extern int g_stop;
extern int g_ttl;

size_t ping_cvt_number(const char *arg, size_t maxval);
void tvsub(struct timeval *out, struct timeval *in);
unsigned short icmp_cksum(char *buffer, size_t bufsize);

struct ping_data *ping_init();
int ping_run(struct ping_data *ping, char *hostname);
int ping_recv(struct ping_data *ping);
int ping_xmit(struct ping_data *ping);

trace_t *trace_init();
int trace_run(trace_t *trace, const int hop);
int trace_xmit(trace_t *trace);
int trace_recv(trace_t *trace);

#endif
