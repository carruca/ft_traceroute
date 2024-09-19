#include "ft_traceroute.h"

int
trace_run(trace_t *trace, const int hop)
{
	fd_set fdset;
	int fdmax, tries, nfds, rc;
	struct timeval timeout, tv_out;
	double triptime;

	fdmax = trace->icmpfd + 1;
	timeout.tv_sec = TRACE_DEFAULT_TIMEOUT;
	timeout.tv_usec = 0;

	printf(" %2d  ", hop);

	for (tries = 0; tries < TRACE_DEFAULT_MAXTRIES; ++tries)
	{
		trace_xmit(trace);

		FD_ZERO(&fdset);
		FD_SET(trace->icmpfd, &fdset);

		fflush(stdout);
		nfds = select(fdmax, &fdset, NULL, NULL, &timeout);
		if (nfds == -1)
			error(EXIT_FAILURE, errno, "select");
		else if (nfds == 0)
			printf(" * ");
		else
		{
			rc = trace_recv(trace);
			if (rc < 0)
			{
				--tries;
				continue ;
			}
			else
			{
				gettimeofday(&tv_out, NULL);
				tvsub(&tv_out, &trace->tsent);

				triptime = ((double)tv_out.tv_sec) * 1000.0 +
					((double)tv_out.tv_usec) / 1000.0;

				if (tries == 0)
					printf(" %s ", inet_ntoa(trace->from_addr.sin_addr));
				printf(" %.3fms ", triptime);
			}
		}
	}
	printf("\n");
	return 0;
}
