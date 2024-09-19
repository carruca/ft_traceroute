#include "ft_traceroute.h"

int
trace_decode_buffer(char *buffer, size_t bufsize,
	struct ip **ipp, struct icmp **icmpp)
{
	unsigned int hlen;
	unsigned short cksum;
	struct ip *ip;
	struct icmp *icmp;

	ip = (struct ip *)buffer;
	hlen = ip->ip_hl << 2;
	icmp = (struct icmp *)(buffer + hlen);

	*ipp = ip;
	*icmpp = icmp;

	cksum = icmp->icmp_cksum;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = icmp_cksum(buffer, bufsize);
	if (icmp->icmp_cksum != cksum)
		return 1;
	return 0;
}

int
trace_recv(trace_t *trace)
{
	char buffer[TRACE_DEFAULT_BUFSIZE];
	size_t bufsize;
	ssize_t nrecv;
	socklen_t from_addrlen;
	struct ip *ip;
	struct icmp *icmp;

	from_addrlen = sizeof(struct sockaddr_in);
	bufsize = sizeof(buffer);
	nrecv = recvfrom(trace->icmpfd, buffer, bufsize, 0,
		(struct sockaddr *)&trace->from_addr, &from_addrlen);
	if (nrecv < 0)
		error(EXIT_FAILURE, errno, "recvfrom");

	trace_decode_buffer(buffer, nrecv, &ip, &icmp);

	if (icmp->icmp_type != ICMP_TIME_EXCEEDED
		&& icmp->icmp_type != ICMP_DEST_UNREACH)
		return -1;

	if (icmp->icmp_type == ICMP_DEST_UNREACH)
		g_stop = 1;

	return 0;
}
