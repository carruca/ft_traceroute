#include "ft_traceroute.h"

unsigned g_ping_options = 0;
int g_stop = 0;
int g_ttl = 0;

static error_t
parse_opt(int key, char *arg,
	struct argp_state *state)
{
	switch(key)
	{
		case 'v':
			g_ping_options |= PING_OPTION_VERBOSE;
			break;

		case ARG_TTL:
			g_ttl = ping_cvt_number(arg, PING_DEFAULT_MAXTTL);
			break;

		case ARGP_KEY_NO_ARGS:
			argp_error(state, "missing host operand");

		/* fallthrough */
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int
main(int argc, char **argv)
{
	int index;
	//struct ping_data *ping;
	trace_t *trace;

	char args_doc[] = "[OPTION...] HOST";
	char doc[] = "Print the route packets trace to network host.";
	struct argp_option argp_options[] = {
//		{"verbose", 'v', NULL, 0, "verbose output", 0},
//		{"ttl", ARG_TTL, "N", 0, "specify N as time-to-alive", 0},
		{0}
	};
	struct argp argp =
		{argp_options, parse_opt, args_doc, doc, NULL, NULL, NULL};

	if (argp_parse(&argp, argc, argv, 0, &index, NULL) != 0)
		return 0;
	
	trace = trace_init();
	if (trace == NULL)
		exit(EXIT_FAILURE);

	if (setsockopt(trace->udpfd, IPPROTO_IP, IP_TTL, &trace->ttl, sizeof(trace->ttl)) < 0)
		error(0, errno, "setsockopt");

	/*
	ping = ping_init();
	if (ping == NULL)
		exit(EXIT_FAILURE);

	if (g_ttl > 0)
		if (setsockopt(ping->fd, IPPROTO_IP, IP_TTL, &g_ttl, sizeof(g_ttl)) < 0)
			error(0, errno, "setsockopt(IP_TTL)");
*/
	argv += index;
	//argc -= index;

	trace_run(trace, *argv);
/*
	while (argc--)
		ping_run(ping, *argv++);

	close(ping->fd);
	free(ping);
	*/
	close(trace->udpfd);
	close(trace->icmpfd);
	free(trace);
	return 0;
}
