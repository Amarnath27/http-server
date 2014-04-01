#define __USE_BSD 	1

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#ifndef INADDR_NONE
#define INADDR_NONE	0xffffffff
#endif

typedef unsigned short u_short;
extern int errno;

int errexit(const char *format, ...);

int 
connectsock(const char *host, const char *service, const char *transport)
{
	struct hostent	*phe;
	struct servent	*pse;
	struct protoent 	*ppe;
	struct sockaddr_in 	sin;

	int s, type;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	if ((pse = getservbyname(service, transport)) > 0)
		sin.sin_port = pse->s_port;
	else if ((sin.sin_port = htons((u_short)atoi(service))) == 0) {
		errexit("can't get \"%s\" service entry\n", service);
	}

	if ((phe = gethostbyname(host)) > 0)
		memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
	else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
		errexit("can't get \"%s\" host entry\n", host);
	}

	if ((ppe = getprotobyname(transport)) == 0) {
		errexit("can't get \"%s\" protocol entry\n", transport);
	}

	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	s = socket(PF_INET, type, ppe->p_proto);

	if (s < 0) {
		errexit("can't create socket: %s\n", strerror(errno));
	}

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		errexit("can't connect to %s.%s: %s\n", host, service, strerror(errno));
	}

	return s;
}
