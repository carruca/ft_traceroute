#include "ft_traceroute.h"

trace_t *
trace_init()
{
	int icmpfd, udpfd;
	struct protoent *proto;
	trace_t *trace;

	udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpfd < 0)
		error(EXIT_FAILURE, errno, "socket");

	proto = getprotobyname("icmp");
	if (proto == NULL)
		error(EXIT_FAILURE, errno, "getprotobyname");

	icmpfd = socket(AF_INET, SOCK_RAW, proto->p_proto);
	if (icmpfd < 0)
		error(EXIT_FAILURE, errno, "socket");

	trace = malloc(sizeof(trace_t));
	if (trace == NULL)
	{
		close(udpfd);
		close(icmpfd);
		return NULL;
	}

	memset(trace, 0, sizeof(*trace));
	trace->udpfd = udpfd;
	trace->icmpfd = icmpfd;
	trace->ttl = TRACE_DEFAULT_FIRSTHOP;
	return trace;
}

struct ping_data *
ping_init()
{
	int fd;
	struct protoent *proto;
	struct ping_data *ping;

	proto = getprotobyname("icmp");
	if (proto == NULL)
	{
		perror("getprotobyname");
		return NULL;
	}

	fd = socket(AF_INET, SOCK_RAW, proto->p_proto);
	if (fd < 0)
	{
		perror("socket");
		return NULL;
	}

	ping = malloc(sizeof(struct ping_data));
	if (ping == NULL)
	{
		close(fd);
		return NULL;
	}

	memset(ping, 0, sizeof(*ping));
	ping->fd = fd;
	ping->type = ICMP_ECHO;
	ping->id = getpid();
	ping->interval = PING_DEFAULT_INTERVAL;
	ping->datalen = PING_DEFAULT_DATALEN;
	ping->count = PING_DEFAULT_COUNT;
	return ping;
}
