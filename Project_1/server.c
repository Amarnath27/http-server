#define _USE_BSD

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

extern int	errno;

#define UDP_SERV	0
#define TCP_SERV	1

#define NOSOCK		-1

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif /* MAX */

#define	QLEN	32
#define LINELEN	128

extern unsigned short portbase;

struct service {
	char *sv_name;
	char sv_useTCP;
	int sv_sock;
	void(*sv_func)(int);
};

char *ROOT;

int errexit(const char *format, ...);
void handle_tcp(int);
void handle_http(int);

int passivesock(const char *service, const char *transport, int qlen);
void reaper(int sig);
void doTCP(struct service *psv);

struct service svent[] = {
	{ "8080", TCP_SERV, NOSOCK, handle_http },
	{ "8081", TCP_SERV, NOSOCK, handle_tcp },
	{0, 0, 0, 0},	
};


int
main(int argc, char *argv[]) {
    struct service	*psv, *fd2sv[NOFILE];
    int			fd, nfds;
    fd_set		afds, rfds;
    char 		c;    

    //Default Values PATH = ~/ and PORT=8080
    char PORT[6];
    ROOT = getenv("PWD");
    strcpy(PORT,"8080");

    //Parsing the command line arguments
    while ((c = getopt (argc, argv, "p:r:")) != -1)
        switch (c) {
            case 'r':
                ROOT = malloc(strlen(optarg));
                strcpy(ROOT,optarg);
                break;
            case 'p':
                strcpy(PORT,optarg);
                break;
            case '?':
                fprintf(stderr,"Wrong arguments given!!!\n");
                exit(1);
            default:
                exit(1);
        }
    
    printf("Server started at port no. %s%s%s with root directory as %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m",ROOT,"\033[0m");

	nfds = 0;
	FD_ZERO(&afds);
	for (psv = &svent[0]; psv->sv_name; ++psv) {
		if (psv->sv_useTCP)
			psv->sv_sock = passivesock(psv->sv_name, "tcp", QLEN);
		else 
			psv->sv_sock = passivesock(psv->sv_name, "udp", 0);

		fd2sv[psv->sv_sock] = psv;
		nfds = MAX(psv->sv_sock + 1, nfds);
		FD_SET(psv->sv_sock, &afds);
	}

	(void)signal(SIGCHLD, reaper);

	while (1) {
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0){
			if (errno == EINTR)
				continue;

			errexit("select error: %s\n", strerror(errno));
		}

		for (fd = 0; fd < nfds; ++fd){
			if (FD_ISSET(fd, &rfds)){
				psv = fd2sv[fd];
				if (psv->sv_useTCP)
				{
					doTCP(psv);
				}
				else
					psv->sv_func(psv->sv_sock);
			}
		}
	}
}

void 
doTCP(struct service *psv){
	struct sockaddr_in	fsin;
	unsigned int		alen;
	int			fd, ssock;

	alen = sizeof(fsin);
	ssock = accept(psv->sv_sock, (struct sockaddr *)&fsin, &alen);
	if (ssock < 0)
		errexit("accept: %s\n", strerror(errno));

	switch (fork()){
	case 0:
		break;
	case -1:
		errexit("fork: %s\n", strerror(errno));
	default:
		(void)close(ssock);
		return;
	}

	for (fd = NOFILE; fd >= 0; --fd)
		if (fd != ssock)
			(void)close(fd);

	psv->sv_func(ssock);
	exit(0);
}

void reaper(int sig){
	int status;
	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0){
		/* empty */	
	}
}
