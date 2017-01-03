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
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "smld_hash.h"
#include "smld_hook.h"
#include "smld_error.h"

/*----------------------------------------------------------------------------*/
static uint64_t (*smldhash_getcountp)(void)     = NULL;
static void     (*smldhash_getlistp)(void*)     = NULL;
static ESTATUS  (*smldhash_remove64p)(uint64_t) = NULL;
static void     (*smldhash_cleanupp)(void)      = NULL;

static void* libsmldso_ptr = NULL;

/*----------------------------------------------------------------------------*/
void smld_start(void)
{
	libsmldso_ptr = dlopen("../lib/libsmldhook.so", RTLD_LAZY);
	if(!libsmldso_ptr)
		handle_error("dlopen ../lib/libsmldhook.so");

	smldhash_getcountp = dlsym(libsmldso_ptr, "smldhash_getcount");
	if(!smldhash_getcountp)
		handle_error("dlysm smldhash_getcount");

	smldhash_getlistp = dlsym(libsmldso_ptr, "smldhash_getlist");
	if(!smldhash_getlist)
		handle_error("dlsym smldhash_getlist");

	smldhash_remove64p = dlsym(libsmldso_ptr, "smldhash_remove64");
	if(!smldhash_remove64p)
		handle_error("dlsym smldhash_remove64");
	
	smldhash_cleanupp = dlsym(libsmldso_ptr, "smldhash_cleanup");
	if(!smldhash_cleanupp)
		handle_error("dlsym smldhash_cleanup");

	int fd = shm_open(SMLDHOOK_SHM_NAME, O_RDWR, 0666);
	if(fd == -1)
		handle_error("shm_open SMLDHOOK");

	smld_enabled = mmap(NULL, SMLDHOOK_SHM_SIZE, PROT_WRITE, MAP_SHARED, fd,0);
	if(smld_enabled == MAP_FAILED)
		handle_error("mmap SMLDHOOK");

	*smld_enabled = 1;
}

/*----------------------------------------------------------------------------*/
uint64_t smld_check(void* ptr)
{
	uint64_t *tmp = ptr;
	if(!smldhash_getcountp || !smldhash_getlistp || !smldhash_remove64p)
		handle_error("smldhash function is null");

	uint64_t count = smldhash_getcountp();

	if(count && tmp != NULL){
		smldhash_getlistp(tmp);
	}

	smldhash_cleanupp();
	return count;
}

/*----------------------------------------------------------------------------*/
void smld_stop(void)
{
	*smld_enabled = 0;
	
	if(munmap((void*)smld_enabled, SMLDHOOK_SHM_SIZE) == -1)
		handle_error("munmap SMLDHOOK");
	dlclose(libsmldso_ptr);
	smldhash_cleanupp();
	smldhash_remove64p = NULL;
	smldhash_getcountp = NULL;
	smldhash_getlistp  = NULL;
	smldhash_cleanupp  = NULL;
}

