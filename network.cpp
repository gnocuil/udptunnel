#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <net/route.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include "network.h"

unsigned mtu;

int set_mtu(char *interface_name, unsigned mtu) {
	int fd;
	if ((fd = socket(PF_INET,SOCK_STREAM,0)) < 0) {
		fprintf(stderr, "set_mtu: Error create socket: %m\n");
		return -1;
	}

	struct ifreq ifr;
	strcpy(ifr.ifr_name,interface_name);
	ifr.ifr_mtu = mtu;

	if(ioctl(fd, SIOCSIFMTU, &ifr) < 0) {
		fprintf(stderr, "set_mtu: Error set %s mtu: %m\n",interface_name);
		return -1;
	}
	fprintf(stderr, "MTU of %s: %d\n", interface_name, mtu);
	return 0;
}

int interface_up(char *interface_name) 
{
	int s;
	if((s = socket(PF_INET,SOCK_STREAM,0)) < 0) {
		fprintf(stderr, "interface_up: Error create socket: %m\n");
		return -1;
	}
	struct ifreq ifr;
	strcpy(ifr.ifr_name,interface_name);
	
	short flag;
	flag = IFF_UP;
	if(ioctl(s, SIOCGIFFLAGS, &ifr) < 0) {
		fprintf(stderr, "interface_up: Error getting flags: %m\n");
		return -1;
	}
	ifr.ifr_ifru.ifru_flags |= flag;
	if(ioctl(s, SIOCSIFFLAGS, &ifr) < 0) {
		fprintf(stderr, "interface_up: Error setting flags: %m\n");
		return -1;
	}
	return 0;
}

