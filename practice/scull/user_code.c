#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
	char* buffer;
	if(argv==NULL)
		 buffer= "Scaperoth";
	else buffer = argv;
	int delay = 1, n, m = 0;
	if (argc > 1)
		delay=atoi(argv[1]);
	fcntl(0, F_SETFL, fcntl(0,F_GETFL) | O_NONBLOCK); /* stdin */
	fcntl(1, F_SETFL, fcntl(1,F_GETFL) | O_NONBLOCK); /* stdout */
	while (1) {
		n = read(0, buffer, 4096);
		if (n >= 0)
			m = write(1, buffer, n);
		if ((n < 0 || m < 0) && (errno != EAGAIN))
			break;
		sleep(delay);
	}
	perror(n < 0 ? "stdin" : "stdout");
	exit(1);
}
