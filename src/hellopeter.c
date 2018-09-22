#include <stdio.h>
#include <stdlib.h>


//#include "serial_test.h"
#include "alsa/asoundlib.h"
#include "alsa_record.h"
#include "alsa_play.h"
#include "alsalib_test.h"
#include "multi_thread.h"
#include "serial_test.h"

#include <pthread.h>


//pthread_mutex_t mutex;



int main(int argc, char *argv[])
{
	//alsa_test(AUDIO_PLAY);
//	alsa_test(AUDIO_RECORD);
	main_test();
//	serial_test(1);
//	serial_test(0);
    return 0;
}   


// int main_test()
// {
    // int tmp1, tmp2;
    // void *retval;
    // pthread_t thread1, thread2;
    // // char *message1 = "thread1";
    // // char *message2 = "thread2";

    // int ret_thrd1, ret_thrd2;
	// pthread_mutex_init(&mutex, NULL);

    // ret_thrd1 = pthread_create(&thread1, NULL, alsa_test, AUDIO_PLAY);
    // ret_thrd2 = pthread_create(&thread2, NULL, alsa_test, AUDIO_RECORD);

    // // 线程创建成功，返回0,失败返回失败号
    // if (ret_thrd1 != 0) {
        // printf("线程1创建失败\n");
    // } else {
        // printf("线程1创建成功\n");
    // }

    // if (ret_thrd2 != 0) {
        // printf("线程2创建失败\n");
    // } else {
        // printf("线程2创建成功\n");
    // }

    // //同样，pthread_join的返回值成功为0
    // tmp1 = pthread_join(thread1, &retval);
    // printf("thread1 return value(retval) is %d\n", (int)retval);
    // printf("thread1 return value(tmp) is %d\n", tmp1);
    // if (tmp1 != 0) {
        // printf("cannot join with thread1\n");
    // }
    // printf("thread1 end\n");

    // tmp2 = pthread_join(thread1, &retval);
    // printf("thread2 return value(retval) is %d\n", (int)retval);
    // printf("thread2 return value(tmp) is %d\n", tmp1);
    // if (tmp2 != 0) {
        // printf("cannot join with thread2\n");
    // }
    // printf("thread2 end\n");

// }




