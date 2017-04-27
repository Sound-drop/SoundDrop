# SoundDrop

### Final Project
### Design using C++
### Bjarne Stroustrup
### Columbia University

AirDrop is a popular peer-to-peer file sharing service developed by Apple for MacOS 
and iOS products.  We propose building a file sharing service using sounds as a 
synchronization (and possibly data transmission, time allowing) method for file 
transfers.  Our application leverages the speaker and microphone readily available 
on almost all consumer devices to transmit the necessary information for scp (
secure copy - a bash utility) to ssh into a target machine and copy a target file 
to its destination.  Note that our application does not extend the capabilities of 
a scp, but rather provides convenience to users.  With SoundDrop, users can simply 
specify a file to send and all devices in the local area (ie. within earshot) will 
automatically connect and copy the file.


#### --- RSA KEY PAIR SETUP ---
For running a secure copy (scp) between two hosts as part of the project, 
host_src and host_dest.

host_src (computer running reciever) is where the program will run the scp command,
host-dest (computer running sender) is where it is going.

On host_src (the one running the scp command) create a key pair for SoundDrop

```
$ ssh-keygen -t rsa -f sounddrop
```

Make sure both keys are in your ~/.ssh directory

Copy sounddrop.pub to host_dest using whatever machinism you like.

On host_dest, copy the contents of sounddrop.pub to ~/.ssh/authorized_keys.

**IMPORTANT**: Create a authorized_keys file if one does not already exist in your .ssh 
directory

```
$ cat id_rsa.pub >>~/.ssh/authorized_keys
$ chmod 700 ~/.ssh/authorized_keys
```

Finally, configure computer settings on host_dest to enable remote login and file 
sharing on a subset of your file system that can be copied from outside systems.

**CAUTION**: Make sure you don't enable sharing on file systems that contain sensitive 
information.

Files can now be transferred using the transfer.cpp program as part of SoundDrop
from host_dest to host_src. 

Above steps must be repeated for opposite direction communication.
