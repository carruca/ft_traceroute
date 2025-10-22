#include "ft_traceroute.h"

unsigned g_ping_options = 0;
int g_stop = 0;
int g_ttl = 0;
int g_first_ttl = 0;
int g_max_ttl = TRACE_DEFAULT_MAXHOPS;

static error_t
parse_opt(int key, char *arg,
	struct argp_state *state)
{
	switch(key)
	{
		case 'f':
			g_first_ttl = trace_cvt_number(arg);
			break;

		case 'm':
			g_max_ttl = trace_cvt_number(arg);
			if (!g_max_ttl)
				argp_error(state, "first hop out of range");
			if (g_max_ttl < 0 || g_max_ttl > 255)
				argp_error(state, "max hop cannot be more than 255");
			break;

		case ARGP_KEY_NO_ARGS:
			argp_error(state, "missing host operand");

		/* fallthrough */
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

void
trace_inc_ttl(trace_t *trace)
{
	++trace->ttl;
	if (setsockopt(trace->udpfd, IPPROTO_IP, IP_TTL, &trace->ttl, sizeof(trace->ttl)) < 0)
		error(0, errno, "setsockopt");
}

void
trace_inc_port(trace_t *trace)
{
	trace->dest_addr.sin_port = htons(ntohs(trace->dest_addr.sin_port) + 1);
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
main(int argc, char **argv)
{
	int index, hop;
	trace_t *trace;

	char args_doc[] = "[OPTION...] HOST";
	char doc[] = "Print the route packets trace to network host.";
	struct argp_option argp_options[] = {
		{"first-hop", 'f', "NUM", 0, "set initial hop distance, i.e., time-to-live", 0},
		{"max-hop", 'm', "NUM", 0, "set maximal hop count (default: 32)", 0},
		{0}
	};
	struct argp argp = {argp_options, parse_opt, args_doc, doc, NULL, NULL, NULL};

	if (argp_parse(&argp, argc, argv, 0, &index, NULL) != 0)
		return 0;
	
	trace = trace_init();
	if (trace == NULL)
		exit(EXIT_FAILURE);

	if (g_first_ttl)
		trace->ttl = g_first_ttl;

	if (setsockopt(trace->udpfd, IPPROTO_IP, IP_TTL, &trace->ttl, sizeof(trace->ttl)) < 0)
		error(0, errno, "setsockopt");

	argv += index;

	if (trace_set_dest(trace, *argv))
		error(EXIT_FAILURE, 0, "unknown host");

	printf("traceroute to %s (%s), %d hops max\n",
		*argv,
		inet_ntoa(trace->dest_addr.sin_addr),
		g_max_ttl);

	hop = 1;

	while(!g_stop)
	{
		if (hop > g_max_ttl)
			exit(EXIT_FAILURE);
		trace_run(trace, hop);
		trace_inc_ttl(trace);
		trace_inc_port(trace);
		++hop;
	}

	exit(EXIT_SUCCESS);
}
