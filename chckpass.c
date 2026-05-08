#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <shadow.h>
#include <string.h>
#include <crypt.h>
#define _XOPEN_SOURCE
int main(int argc, char ** argv)
{
	char * input_pass;
	struct spwd* shadow_line;
	char * crypted_pass;
	if(argc <2)
	{
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	input_pass = getpass("Passwd:");
	shadow_line = getspnam(argv[1]);
        
	//参数：1  加密对象    2  盐值
	crypted_pass = crypt(input_pass, shadow_line->sp_pwdp);//这里将影子文件中的加密密钥拿了过来，会取从 $id$ 之后到下一个 $ 之间的内容作为盐值
	if(strcmp(shadow_line->sp_pwdp, crypted_pass) == 0)
	{
		puts("OK");
	}
	else
	{
		puts("failed");
	}
	exit(0);

}
