#include "ft_traceroute.h"

void
ping_print_stat(struct ping_data *ping, char *hostname)
{
	double avg;
	double vari;

	printf("--- %s ping statistics ---\n", hostname);
	printf("%lu packets transmitted, ", ping->num_xmit);
	printf("%lu packets received", ping->num_recv);

	if (ping->num_xmit)
	{
		printf(", %d%% packet loss",
			(int)((ping->num_xmit - ping->num_recv) * 100 / ping->num_xmit));
	}
	printf("\n");

	if (ping->num_recv)
	{
		avg = ping->ping_stat.tsum / ping->num_recv;
		vari = ping->ping_stat.tsumsq / ping->num_recv - avg * avg;

		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
		ping->ping_stat.tmin,
		avg,
		ping->ping_stat.tmax,
		sqrt(vari));
	}
}

void
ping_reset_data(struct ping_data *ping)
{
	ping->ping_stat.tmin = 0.0;
	ping->ping_stat.tmax = 0.0;
	ping->ping_stat.tsum = 0.0;
	ping->ping_stat.tsumsq = 0.0;

	ping->num_xmit = 0;
	ping->num_recv = 0;

	free(ping->buffer);
	ping->buffer = NULL;
}

void
sig_handler(int signal)
{
	if (signal == SIGINT)
		g_stop = 1;
}

int
ping_loop(struct ping_data *ping, char *hostname)
{
	fd_set fdset;
	int fdmax, nfds;
	struct timeval timeout;

	signal(SIGINT, sig_handler);

	if (ping_xmit(ping))
		error(EXIT_FAILURE, errno, "sending packet");

	fdmax = ping->fd + 1;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	while (!g_stop)
	{
		FD_ZERO(&fdset);
		FD_SET(ping->fd, &fdset);

		nfds = select(fdmax, &fdset, NULL, NULL, &timeout);
		if (nfds == -1)
			error(EXIT_FAILURE, errno, "select failed");
		else
		{
			
			/*
			if (nfds == 1 && ping_recv(ping))
				continue;

			usleep(ping->interval * 1000);
			if (!g_stop && ping_xmit(ping))
				error(EXIT_FAILURE, errno, "sending packet");
			*/
		}
	}

	ping_print_stat(ping, hostname);
	ping_reset_data(ping);
	return 0;
}

int
ping_set_dest(struct ping_data *ping, const char *hostname)
{
	int rc;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_CANONNAME;

	rc = getaddrinfo(hostname, NULL, &hints, &res);
	if (rc != 0)
		return 1;
	memcpy(&ping->dest_addr, res->ai_addr, res->ai_addrlen);

	freeaddrinfo(res);
	return 0;
}

int
ping_run(struct ping_data *ping, char *hostname)
{
	if (ping_set_dest(ping, hostname))
		error(EXIT_FAILURE, 0, "unknown host");

	printf("PING %s (%s): %zu data bytes",
		hostname,
		inet_ntoa(ping->dest_addr.sin_addr),
		ping->datalen);
	if (g_ping_options & PING_OPTION_VERBOSE)
		printf(", id 0x%x = %i", ping->id, ping->id);
	printf("\n");

	return ping_loop(ping, hostname);
}

int
trace_set_dest(trace_t *trace, const char *hostname)
{
	int rc;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_CANONNAME;

	rc = getaddrinfo(hostname, NULL, &hints, &res);
	if (rc != 0)
		return 1;
	memcpy(&trace->dest_addr, res->ai_addr, res->ai_addrlen);
	trace->dest_addr.sin_port = htons(TRACE_DEFAULT_UDPPORT);

	freeaddrinfo(res);
	return 0;
}

int
trace_run(trace_t *trace, char *hostname)
{
	fd_set fdset;
	int fdmax, tries, nfds;
	struct timeval timeout;
	int hop;

	if (trace_set_dest(trace, hostname))
		error(EXIT_FAILURE, 0, "unknown host");

	printf("traceroute to %s (%s), %d hops max\n",
		hostname,
		inet_ntoa(trace->dest_addr.sin_addr),
		TRACE_DEFAULT_MAXHOPS);

	hop = 1;
	fdmax = trace->icmpfd + 1;
	timeout.tv_sec = TRACE_DEFAULT_TIMEOUT;
	timeout.tv_usec = 0;

	printf(" %2d  ", hop);

	for (tries = 0; tries < TRACE_DEFAULT_MAXTRIES; ++tries)
	{
		trace_xmit(trace);

		FD_ZERO(&fdset);
		FD_SET(trace->icmpfd, &fdset);

		nfds = select(fdmax, &fdset, NULL, NULL, &timeout);
		if (nfds == -1)
			error(EXIT_FAILURE, errno, "select");
		else
		{

		}
	}

	return 0;
}
