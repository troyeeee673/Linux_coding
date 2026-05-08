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
	passwdline = getpwuid(atoi(argv[1]));
	puts(passwdline -> pw_name);
	exit(0);
}
