#include <iostream>
#include <string>
#include <cstdlib>

#define LIBSSH_STATIC 1 // needed for library linking per instruction
#include <libssh/libssh.h>

using namespace std;

int main (int argc, char **argv) {
	// direct system call to run a secure copy (scp)

	string remote, path, command;

	if (argc != 3) {
        cout << "Usage: ./transfer <USERNAME@REMOTE_ADDRESS> <FILE_PATH>" << endl;
        return 1;
    }

    remote = argv[1];
    path = argv[2];
    command = "scp -i ~/.ssh/sounddrop " + remote + ":~" + path + " . ";

    system(command.c_str());

    return 0;
}