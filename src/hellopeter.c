#include "alsalib_test.h"
#include "serial_test.h"

int main(void)
{
	alsa_play();
	serial_test(0);

}   

/*

int main(int argc, char *argv[])
{
    int i;

    printf("Peter say: aaa\n");

    for (i=0;i<argc;i++)
    {
       printf("argv[%d]=%s\n",i,argv[i]);
    }

    return 0;
}


*/

