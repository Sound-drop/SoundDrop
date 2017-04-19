#include <iostream>
#include <string>
#include "portaudio.h"
#include "aquila.h"

#define LIBSSH_STATIC 1 // needed for library linking per instruction
#include <libssh/libssh.h>

using namespace std;

int main (int argc, char **argv) {
	// check arguments
	if (argc != 3) {
        cout << "Usage: ./transfer <DESTINATION> <FILE_PATH>" << endl;
        return 1;
    }

    ssh_session my_ssh_session = ssh_new();
    cout << "SSH session created" << endl;
  	if (my_ssh_session == NULL)
    	return 1;

  	ssh_free(my_ssh_session);
  	cout << "SSH session ended" << endl;

	return 0;
}