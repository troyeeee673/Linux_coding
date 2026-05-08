#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, char** argv)
{
	struct passwd * passwdline;
	if(argc < 2)
	{
		fprintf(stderr, "Usage...\n");
		exit(1);
	}
	passwdline = getpwnam(argv[1]);
	printf("%u\n",passwdline -> pw_uid);
	exit(0);
}
