#include <stdio.h>
#include <stdlib.h>


//#include "serial_test.h"
#include "alsa/asoundlib.h"
#include "alsa_record.h"
#include "alsa_play.h"
#include "alsalib_test.h"
#include "multi_thread.h"
#include "serial_test.h"
#include "gpio.h"
#include <pthread.h>




int main(int argc, char *argv[])
{
	GPIONumber gpioao_8 = gpio505;
	GPIONumber gpioao_3 = gpio500;
	//alsa_test(AUDIO_PLAY);
//	alsa_test(AUDIO_RECORD);
	gpio_test(gpioao_3);
	main_test();
//	serial_test(1);
//	serial_test(0);
    return 0;
}   






