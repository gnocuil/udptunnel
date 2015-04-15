#include <linux/if_tun.h>
#include <net/if.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/io.h>
#include <iostream>
#include <netinet/ip6.h>  
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>

#include "tun.h"
#include "socket.h"
#include "network.h"

static int tun_fd;
static char buf[2040];

struct in_addr addr_local, addr_remote;
unsigned short port_local, port_remote;

int tun_create(char *dev)
{
	struct ifreq ifr;
	int err;

	if ((tun_fd = open("/dev/net/tun", O_RDWR)) < 0) {
		fprintf(stderr, "tun_create: Error Creating TUN/TAP: %m\n");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags |= IFF_TUN | IFF_NO_PI;

	if (*dev != '\0') {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}

	if ((err = ioctl(tun_fd, TUNSETIFF, (void *)&ifr)) < 0) {
		fprintf(stderr, "tun_create: Error Setting tunnel name %s: %m\n", dev);
		close(tun_fd);
		return -1;
	}
/*
	if (fcntl(tun_fd, F_SETFL, O_NONBLOCK) < 0) {
		fprintf(stderr, "tun_create: Error Setting nonblock: %m\n", dev);
		return -1;
	}
*/
	
	strcpy(dev, ifr.ifr_name);
	
	return tun_fd;
}

int tun_send(char *packet, int len)
{
	int count = write(tun_fd, packet, len);
	if (count != len) {
		fprintf(stderr, "tun_send : Error sending len=%d count=%d: %m\n", len, count);
		return -1;
	}
	return 0;
}

int handle_tun(char *buf, int len)
{
	int count = read(tun_fd, buf, len);
	return count;
}
