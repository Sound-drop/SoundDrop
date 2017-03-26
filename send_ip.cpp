#include <stdio.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <iostream>

#include "portaudio.h"

using namespace std;

string getIP() {
	string addr;
	struct ifaddrs *ifAddrStruct;

	getifaddrs(&ifAddrStruct);

	for (struct ifaddrs *ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
			char str[INET_ADDRSTRLEN];
			struct in_addr *tmp = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmp, str, INET_ADDRSTRLEN);

			// Discard localhost
			if (strcmp(str, "127.0.0.1") != 0) {
				addr = str;
			}
		}
	}

	if (ifAddrStruct != NULL) {
		freeifaddrs(ifAddrStruct);
	}

	return addr;
}

int main (int argc, char **argv) {
	string addr = getIP();
	cout << "IPv4 Address: " << addr << endl;

	

	return 0;
}


