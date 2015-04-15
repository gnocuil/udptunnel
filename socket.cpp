#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>  
#include <netinet/udp.h> 
#include <string.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>

#include "socket.h"
#include "tun.h"
#include "network.h"

static int raw_fd;

static int udp_fd;
static char buf[2000];

static struct sockaddr_in servaddr;

int socket_init()
{
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port_local);
	bind(udp_fd, (struct sockaddr *)&servaddr,sizeof(servaddr));
	
	servaddr.sin_addr = addr_remote;
	servaddr.sin_port = htons(port_remote);
	return udp_fd;
}

int handle_socket(char *buf, int len)
{
    int count = recvfrom(udp_fd, buf, len, 0, 0, 0);
	return count;
}

int socket_send(char *buf, int len)
{
	sendto(udp_fd, buf, len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
}
