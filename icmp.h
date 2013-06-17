#pragma once
#include "encap.h"

class Encap_ICMP : public Encap {
public:
	const char* name() { return "ICMP"; }
	char* readbuf() {
		return buf + 40;
	}
	int readbuflen() {
		return BUF_LEN - 40;
	}
	char* sendbuf() {
		return buf;
	}
	int makepacket(int len) {
		return 0;
	}
	int init_socket() {
		return 0;
	}
	int handle_socket() {
		return 0;
	}
	char* send4buf() {
		return buf4;
	}
private:
	char buf[BUF_LEN];
	char buf4[BUF_LEN];
};
