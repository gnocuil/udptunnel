#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <linux/if_tun.h>
#include <iostream>
#include <arpa/inet.h>

#include "tun.h"
#include "network.h"
#include "socket.h"

using namespace std;

#define DEFAULT_MTU 1460

static void usage()
{
	fprintf(stderr, "Usage: tunnel [options] <TI_IPv6_ADDR> <TC_IPv6_ADDR>\n");
	fprintf(stderr, "  options: --name <TUNNEL_NAME>       default: 4over6\n");
	fprintf(stderr, "           --mtu <MTU_VALUE>          default: %d\n", DEFAULT_MTU);
	
	exit(1);
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	char tun_name[IFNAMSIZ] = {0};
	strncpy(tun_name, TUNNEL_NAME, IFNAMSIZ);
	mtu = DEFAULT_MTU;
	
	if (argc < 3)
		usage();
	for (int i = 1; i < argc - 2; ++i) {
		if (strcmp(argv[i], "--help") == 0) {
			usage();
		}
		if (i + 1 < argc - 2 && strcmp(argv[i], "--name") == 0) {
			strncpy(tun_name, argv[++i], IFNAMSIZ);
		}
		if (i + 1 < argc - 2 && strcmp(argv[i], "--mtu") == 0) {
			++i;
			sscanf(argv[i], "%d", &mtu);
		}
	}
	printf("TI_IPv6_ADDR: %s\nTC_IPv6_ADDR: %s\n", argv[argc - 2], argv[argc - 1]);
	inet_pton(AF_INET6, argv[argc - 2], &addr6_TI);
	inet_pton(AF_INET6, argv[argc - 1], &addr6_TC);

	//Create TUN/TAP interface
	int tun_fd = tun_create(tun_name);
	if (tun_fd < 0) {
		exit(1);
	}
	fprintf(stderr, "interface name: %s\n", tun_name);

	set_mtu(tun_name, mtu);//set mtu
	interface_up(tun_name);//interface up

	int pid = fork();
	if (pid == 0) {//son
		int raw_fd = socket_init();
		if (raw_fd < 0) {
			exit(1);
		}
		while (1)
			handle_socket();
	}
	//father
	socket_init_tun();
/*
	int raw_fd = socket_init();
	if (raw_fd < 0) {
		exit(1);
	}
*/	

	fd_set set;
	int maxsock = tun_fd;
//	if (raw_fd > maxsock)
//		maxsock = raw_fd;
	while (1) {
		FD_ZERO(&set);
		FD_SET(tun_fd, &set);
//		FD_SET(raw_fd, &set);
		
		int ret = select(maxsock + 1, &set, NULL, NULL, NULL);
		
		if (ret < 0) {
			fprintf(stderr, "main: Error in select: %m\n", errno);
			break;
		}
		if (FD_ISSET(tun_fd, &set)) {
//			printf("select: TUN!!!\n");
			handle_tun();
		}
/*
		if (FD_ISSET(raw_fd, &set)) {
//			printf("select: RAW!!!\n");
			handle_socket();
		} 
*/
	}

	return 0;
}
