/*************************************************************************
    > File Name: thread_hello_world.c 
    > Author: couldtt(fyby)
    > Mail:  fuyunbiyi@gmail.com
    > Created Time: 2013年12月14日 星期六 11时48分50秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "multi_thread.h"

pthread_mutex_t mutex;

int main_test()
{
    int tmp1, tmp2;
    void *retval;
    pthread_t thread1, thread2;
    int id1 = 1;
    int id2 = 2;

    int ret_thrd1, ret_thrd2;
	pthread_mutex_init(&mutex, NULL);

    ret_thrd1 = pthread_create(&thread1, NULL, &thread_alsa_test, (void*)&id1);
    ret_thrd2 = pthread_create(&thread2, NULL, &thread_alsa_test, (void*)&id2);

    // 线程创建成功，返回0,失败返回失败号
    if (ret_thrd1 != 0) {
        printf("线程1创建失败\n");
    } else {
        printf("线程1创建成功\n");
    }

    if (ret_thrd2 != 0) {
        printf("线程2创建失败\n");
    } else {
        printf("线程2创建成功\n");
    }

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
	while(1)
	{
		sleep(1);
	}

}


void *print_message_function( void *arg ) {
    int i = 0;
    for (i; i<5; i++) {
        printf("%d, %d\n", *((int*)arg), i);
		sleep(1);
    }
	return ((void*)0);
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

    // ret_thrd1 = pthread_create(&thread1, NULL, print_message_function1, AUDIO_PLAY);
    // ret_thrd2 = pthread_create(&thread2, NULL, print_message_function2, AUDIO_RECORD);

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

// void *print_message_function1(void *arg ) {
    // int i = 0;
    // for (i; i<5; i++) {
		// pthread_mutex_lock(&mutex);
        // printf("hi-1---------\n");
		// pthread_mutex_unlock(&mutex);
    // }
	// return ((void*)0);
// }

// void *print_message_function2(void *arg ) {
    // int i = 0;
    // for (i; i<5; i++) {
		// pthread_mutex_unlock(&mutex);
        // printf("hi-2\n");
		// pthread_mutex_lock(&mutex);
    // }
	// return ((void*)0);
// }
