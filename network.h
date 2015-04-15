#pragma once

extern unsigned mtu;

int set_mtu(char *interface_name, unsigned mtu);
int interface_up(char *interface_name);

