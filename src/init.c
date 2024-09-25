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
