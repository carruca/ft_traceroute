#include "ft_traceroute.h"

int
trace_xmit(trace_t *trace)
{
	ssize_t nsent;
	char data[] = "SUPERMAN";

	nsent = sendto(trace->udpfd, (char *)data, sizeof(data), 0,
		(struct sockaddr *)&trace->dest_addr, sizeof(struct sockaddr_in));
	if (nsent < 0)
		error(EXIT_FAILURE, errno, "sendto");

	if (gettimeofday(&trace->tsent, NULL) < 0)
		error(EXIT_FAILURE, errno, "gettimeofday");
	return 0;
}
