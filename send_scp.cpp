#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "sound_drop.h"

using namespace std;

typedef struct { 
	uint8_t n[4]; 
} IP;

IP getIP() {
	string str_addr;
	struct ifaddrs *ifAddrStruct;

	getifaddrs(&ifAddrStruct);

	for (struct ifaddrs *ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
			char str[INET_ADDRSTRLEN];
			struct in_addr *tmp = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmp, str, INET_ADDRSTRLEN);

			// Discard localhost
			if (strcmp(str, "127.0.0.1") != 0) {
				str_addr = str;
			}
		}
	}

	if (ifAddrStruct != NULL) {
		freeifaddrs(ifAddrStruct);
	}

	cout << str_addr << endl;

	/* Convert string ip address to int */
	IP addr;
	stringstream ss(str_addr);
	string tok;

	int i = 0;
	while (getline(ss, tok, '.')) {
		addr.n[i++] = (uint8_t) stoi(tok, nullptr);
	}

	return addr;
}

int main (int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "%s [path_to_file]\n", argv[0]);
		return 1;
	}

	/* Get IP address */
	IP addr = getIP();
	Packet p1 = {
		.len = sizeof(IP),
		.data = (void *) &addr
	};

	/* Get username */
	char buf[30];
	getlogin_r(buf, sizeof(buf));

	Packet p2 = { 
		.len = static_cast<uint16_t>(strlen(buf) + 1), 
		.data = (void *) buf
	};

	/* Get filepath */
	Packet p3 = { 
		.len = static_cast<uint16_t>(strlen(argv[1]) + 1), 
		.data = (void *) argv[1]
	};

	/* Create data stream */
	vector<Packet> data;
	data.push_back(p1);
	data.push_back(p2);
	data.push_back(p3);

	/* Initialize SoundDrop */
	cout << "INFO: Initializing SoundDrop..." << endl;
	SoundDrop sd;

	cout << "INFO: Building transmission..." << endl;
	sd.load(data);

	/* Send transmission */
	cout << "INFO: Sending transmission..." << endl;
	sd.send();

	cout << "INFO: Done! Exiting..." << endl;
	return 0;
}


