#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
static jmp_buf  save;
int d()
{
        printf("%s():Begin.\n", __FUNCTION__);
	printf("%s():Jump now!.\n", __FUNCTION__);
	longjmp(save, 16);
        printf("%s():End.\n");
        return 0;
}

int c()
{
        printf("%s():Begin.\n", __FUNCTION__);
        printf("%s():call d().\n", __FUNCTION__);
        d();
        printf("%s():d() returned.\n", __FUNCTION__);
        printf("%s():End.\n");
        return 0;
}
int b()
{
        printf("%s():Begin.\n", __FUNCTION__);
        printf("%s():call c().\n", __FUNCTION__);
        c();
        printf("%s():c() returned.\n", __FUNCTION__);
        printf("%s():End.\n");
        return 0;
}
int a()
{
	int ret ;
        printf("%s():Begin.\n", __FUNCTION__);
        ret = setjmp(save);
	if(ret == 0)
	{
		printf("%s():call b().\n", __FUNCTION__);
        	b();
        	printf("%s():b() returned.\n", __FUNCTION__);
	}
	else
	{
		printf("%s():Jump back here with code %d\n", __FUNCTION__, ret);
	}
	printf("%s():End.\n", __FUNCTION__);
        return 0;
}

int main()
{
	printf("%s():Begin.\n", __FUNCTION__);
	printf("%s():call a().\n", __FUNCTION__);
	a();
	printf("%s():a() returned.\n", __FUNCTION__);
	printf("%s():End.\n",__FUNCTION__);
	exit(0);
}
