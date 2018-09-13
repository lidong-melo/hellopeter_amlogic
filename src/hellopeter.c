#include <stdio.h>
#include <stdlib.h>


//#include "serial_test.h"
#include "alsa/asoundlib.h"
#include "alsa_record.h"
#include "alsa_play.h"
#include "alsalib_test.h"




int main(void)
{
	alsa_play();
	//alsa_play_test();
	//serial_test(0);
    // record_params_t aa;
    // aa.duration = 10;
    // aa.format = SND_PCM_FORMAT_S16_LE;
    // aa.rate = 48000;
    // aa.channel = 2;
    // char str[20] = "test_2.wav";
    // if (do_alsa_record(str, &aa) == RECORD_SUCCESS)
    // {
        // printf("record sucess!!\n");
        // return 0;
    // }
    // printf("record fail!!\n");
    return 0;
}   


