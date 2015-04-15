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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include "tun.h"
#include "network.h"
#include "socket.h"

using namespace std;

#define DEFAULT_MTU 1472

const int BUF_LEN = 4000;

static void usage()
{
	fprintf(stderr, "Usage: tunnel [options] <LOCAL_UDP_PORT> <REMOTE_IPv4_ADDR> <REMOTE_UDP_PORT>\n");
	fprintf(stderr, "  options: --name <TUNNEL_NAME>       default: 4over6\n");
	fprintf(stderr, "           --mtu <MTU_VALUE>          default: %d\n", DEFAULT_MTU);
	
	exit(1);
}

static void* process_udp_read(void* arg)
{
    char buf[BUF_LEN];
	while (1) {
		int count = handle_socket(buf, BUF_LEN);
		if (count) {
		    std::reverse(buf, buf + count);
		    for (int i = 0; i < count; ++i) buf[i] = ~buf[i];
            tun_send(buf, count);
		}
	}
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
		} else if (i + 1 < argc - 2 && strcmp(argv[i], "--mtu") == 0) {
			++i;
			sscanf(argv[i], "%d", &mtu);
		}
	}
	printf("REMOTE_IP_ADDR: %s\n", argv[argc - 2]);
	inet_pton(AF_INET, argv[argc - 2], &addr_remote);
	int t;
	sscanf(argv[argc - 3], "%d", &t);
	port_local = (unsigned short)t;
	sscanf(argv[argc - 1], "%d", &t);
	port_remote = (unsigned short)t;

	//Create TUN/TAP interface
	tun_create(tun_name);

	fprintf(stderr, "interface name: %s\n", tun_name);

	set_mtu(tun_name, mtu);//set mtu
	interface_up(tun_name);//interface up
	
	//init udp socket
	socket_init();

    pthread_t tid;
    pthread_create(&tid, NULL, process_udp_read, NULL);
    
    char buf[BUF_LEN];
	
	while (1) {
        int count = handle_tun(buf, BUF_LEN);
        if (count) {
            std::reverse(buf, buf + count);
		    for (int i = 0; i < count; ++i) buf[i] = ~buf[i];
            socket_send(buf, count);
        }
    }
}
