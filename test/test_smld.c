/**
********************************************************************************
Copyright (C) 2016 b20yang 
---
This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#define _GNU_SOURCE
#include<stdio.h>
#include<dlfcn.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include<pthread.h>
#include<sched.h>
#include<malloc.h>
#include"smld.h"
#include"test_smld.h"
#include"smld_error.h"

//TC1: basic malloc/free
//----------------------------------------------------------
static void testcase_1(void){
    void* ptr = (void*)malloc(10);
    free(ptr);

    ptr = (void*)malloc(10);
    free(ptr);
    uint64_t lc = smld_check(NULL);
    EXPECTED("TC1", lc, 0);
}

//TC2: two processes
//----------------------------------------------------------
static void testcase_2(void){
    void* pa[10];
    pid_t id = fork();
    if(id == 0)
    {//child
        void*ptr = (void*)malloc(10);
        exit(0);
    }

    //parent
    int status;
    pid_t id1=wait(&status);
    
    uint64_t tmp[20];
    uint64_t lc = smld_check(tmp);
    EXPECTED("TC2",lc, 1);
}

//TC3: two threads
//----------------------------------------------------------
static void* thread_start(void*arg)
{
    void* ptr = (void*)malloc(10);
    free(ptr);
    return NULL;
}
static void testcase_3(void){
    pthread_t tid;

    if(pthread_create(&tid, NULL, thread_start, NULL)){
        handle_error("pthread_create");
    }

    if(pthread_join(tid, NULL)){
        handle_error("pthread_join");
    }
    uint64_t lc = smld_check(NULL);
    EXPECTED("TC3", lc, 0);
}

//TC4: calloc  
//----------------------------------------------------------
static void testcase_4(void){
    void* ptr = (void*)malloc(10);
    free(ptr);

    ptr = (void*)calloc(10, sizeof(uint64_t));
    free(ptr);
    uint64_t lc = smld_check(NULL);
    EXPECTED("TC4", lc, 0);
}

//TC5: memalign  
//----------------------------------------------------------
static void testcase_5(void){
    void* ptr = memalign(64, 10);
    free(ptr);

    uint64_t lc = smld_check(NULL);
    EXPECTED("TC5", lc, 0);
}

//TC6: realloc  
//----------------------------------------------------------
static void testcase_6(void){
    void* ptr = malloc(10);
    void* rptr = realloc(ptr, 20);
    //free(ptr);

    uint64_t tmp[20];

    uint64_t lc = smld_check(tmp);
    EXPECTED("TC6", lc, 1);
}

//TC7: concurrent 
//----------------------------------------------------------
#define TC7_THREAD_NUM 8
typedef struct s_test_arg
{
    uint32_t core;
    uint32_t seed;
}s_test_arg;
static void* thread_start_tc7(void*arg)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    s_test_arg *a = (s_test_arg*)arg;

    CPU_SET(a->core, &cpuset);
    pthread_t tid = pthread_self();

    if(pthread_setaffinity_np(tid, sizeof(cpuset), &cpuset))
        handle_error("pthread_setaffinity_np");
    for(int i = 0; i < 1000; i++)
    {
        usleep(rand_r(&(a->seed))%10);
        void* ptr = (void*)malloc(10);
        usleep(rand_r(&(a->seed))%10);
        free(ptr);
    }
    return NULL;
}

//TC7: concurrent 
//----------------------------------------------------------
static void testcase_7(void){
    pthread_t tid[TC7_THREAD_NUM];
    s_test_arg args[TC7_THREAD_NUM];
    srand(time(NULL));

    for(uint32_t i = 0; i < TC7_THREAD_NUM; i++)
    {
        args[i].core = i % 4;
        if(pthread_create(&tid[i], NULL, thread_start_tc7, (void*)&args[i])){
            handle_error("pthread_create");
        }
    }

    for(uint32_t i = 0; i < TC7_THREAD_NUM; i++)
    {
        if(pthread_join(tid[i], NULL)){
            handle_error("pthread_join");
        }
    }

    uint64_t lc = smld_check(NULL);
    EXPECTED("TC7", lc, 0);
}

void main(void)
{
    smld_start();

    testcase_1();
    testcase_2();
    testcase_3();
    testcase_4();
    testcase_5();
    testcase_6();
    testcase_7();
    
    smld_stop();
}
