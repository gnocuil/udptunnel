#pragma once

#define TUNNEL_NAME "4over6"

int tun_create(char *dev);
int tun_send(char *packet, int len);
int handle_tun();

extern struct in6_addr addr6_TI, addr6_TC;
