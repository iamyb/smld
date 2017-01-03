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
#include <stdio.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "smld_hash.h"
#include "smld_hook.h"
#include "smld_error.h"

/*---------------------------------------------------------------------------*/
static void* (*callocp)(size_t, size_t)   = NULL;
static void* (*mallocp)(size_t)           = NULL;
static void* (*reallocp)(void*, size_t)   = NULL;
static void* (*memalignp)(size_t, size_t) = NULL;
static void  (*freep)(void*)              = NULL;

static void __attribute__((constructor))smldhook_create_shm(void)
{
    int fd = shm_open(SMLDHOOK_SHM_NAME, O_CREAT|O_RDWR|O_EXCL,0666);
    if(fd == -1) 
    {
        if((fd = shm_open(SMLDHOOK_SHM_NAME, O_RDWR, 0666))==-1)
            handle_error("shm_open");
    }

    if(ftruncate(fd, SMLDHOOK_SHM_SIZE) == -1)
        handle_error("ftruncate");

    smld_enabled=mmap(NULL, SMLDHOOK_SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if(smld_enabled == MAP_FAILED)
        handle_error("mmap");
}

static void __attribute__((destructor))smldhook_destroy_shm(void)
{
    if(munmap((void*)smld_enabled, SMLDHOOK_SHM_SIZE) == -1)
        handle_error("munmap");

    smld_enabled = NULL;
    
    if(shm_unlink(SMLDHOOK_SHM_NAME) == -1);
    //    handle_error("shm_unlink");
}

static void __attribute__((constructor))smldhook_init_hook(void)
{
    callocp = (void*(*)(size_t, size_t))dlsym(RTLD_NEXT,"calloc");
    mallocp = (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc");
    reallocp = (void*(*)(void*, size_t))dlsym(RTLD_NEXT,"realloc");
    memalignp=(void*(*)(size_t,size_t))dlsym(RTLD_NEXT,"memalign");
    freep = (void(*)(void*))dlsym(RTLD_NEXT,"free");
}

/*---------------------------------------------------------------------------*/
void* malloc(size_t len)
{
    void* ptr = (*mallocp)(len);
    if(smld_enabled!=NULL&&*smld_enabled && ptr!=NULL){
        smldhash_put64((uint64_t)ptr, (uint64_t)ptr);
    }
    return ptr;
}

/*---------------------------------------------------------------------------*/
//this is a workaround for pthread
static int calloc_buf[1000];
static void* calloc_wa(size_t nmemb, size_t size)
{
    return (void*)calloc_buf;
}

/*---------------------------------------------------------------------------*/
void* calloc(size_t nmemb, size_t size)
{
    if(callocp == NULL)
        return calloc_wa(nmemb, size);

    void *ptr = (*callocp)(nmemb, size);
#if 0
    // calloc just call malloc to allocate memory
    if(smld_enabled!=NULL&&*smld_enabled && ptr!=NULL){
        smldhash_put64((uint64_t)ptr, (uint64_t)ptr);
    }
#endif
    return ptr;
}

/*---------------------------------------------------------------------------*/
void* memalign(size_t alignment, size_t size)
{
    void *ptr = (*memalignp)(alignment, size);
    if(smld_enabled!=NULL&&*smld_enabled && ptr!=NULL){
        smldhash_put64((uint64_t)ptr, (uint64_t)ptr);
    }
    return ptr;
}

/*---------------------------------------------------------------------------*/
void* realloc(void*ptr, size_t size)
{
    void *tptr = (*reallocp)(ptr, size);
    if(smld_enabled!=NULL&&*smld_enabled && tptr!=NULL){
        smldhash_put64((uint64_t)tptr, (uint64_t)tptr);
    }
    return tptr;
}

/*---------------------------------------------------------------------------*/
void free(void* ptr)
{
    if(smld_enabled!=NULL&&*smld_enabled && ptr!=NULL){
        smldhash_remove64((uint64_t)ptr);
    }
    return (*freep)(ptr);
}
