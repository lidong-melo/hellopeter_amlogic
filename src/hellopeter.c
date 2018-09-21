#include <stdio.h>
#include <stdlib.h>


//#include "serial_test.h"
#include "alsa/asoundlib.h"
#include "alsa_record.h"
#include "alsa_play.h"
#include "alsalib_test.h"
#include "multi_thread.h"
#include "serial_test.h"




int main(void)
{
	//alsa_test(AUDIO_PLAY);
	alsa_test(AUDIO_RECORD);
//	serial_test(1);
//	serial_test(0);
    return 0;
}   


