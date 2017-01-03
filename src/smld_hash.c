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
#include<stdint.h>
#include<string.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>

#include"smld_hash.h"
#include"smld_atomic64.h"
#include"smld_error.h"

#define BIG_CONSTANT(x) (x##LLU)

typedef struct s_entry{
	volatile uint64_t key;
	volatile uint64_t value;
}s_entry;

#define SMLDHASH_SHM_NAME "/smldhash_shm"
#define SMLDHASH_SHM_SIZE (BULK_SIZE*sizeof(s_entry)+sizeof(uint64_t))
static s_entry* hash_table=NULL;
//static volatile uint32_t *g_entry_count = NULL;
static smld_atomic64_t *g_entry_count=NULL;
/*----------------------------------------------------------------------------*/
static void __attribute__((constructor))smldhash_create_shm(void)
{
	int fd = shm_open(SMLDHASH_SHM_NAME, O_CREAT|O_RDWR|O_EXCL,0666);
	if(fd == -1){
		if((fd = shm_open(SMLDHASH_SHM_NAME, O_RDWR, 0666))==-1){
			handle_error("shm_open");
		}
	}

	if(ftruncate(fd, SMLDHASH_SHM_SIZE) == -1)
		handle_error("ftruncate");

	uint64_t *ptr = 
		mmap(NULL, SMLDHASH_SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(ptr == MAP_FAILED)
		handle_error("mmap");

	memset(ptr, 0, SMLDHASH_SHM_SIZE);
	g_entry_count = (smld_atomic64_t*)ptr++;
	hash_table = (s_entry*)ptr;
	smld_atomic64_init(g_entry_count);
}

static void __attribute__((destructor))smldhash_destroy_shm(void)
{
	hash_table = NULL;
	if(munmap((void*)g_entry_count, SMLDHASH_SHM_SIZE) == -1)
		handle_error("munmap");
	g_entry_count = NULL;
	if(shm_unlink(SMLDHASH_SHM_NAME) == -1);
		//handle_error("shm_unlink");
}

/*----------------------------------------------------------------------------*/
static uint64_t integer_hash64(uint64_t k)
{
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xff51afd7ed558ccd);
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
	k ^= k >> 33;

	return k;
}

ESTATUS smldhash_put64(uint64_t key, uint64_t value){
	if(g_entry_count==NULL||hash_table==NULL)
		return ESTATUS_NOT_INITIALIZED;
	int count = 0;
	for(int i = integer_hash64(key); count++ < BULK_SIZE; i++){
		i &= (BULK_SIZE-1);
		if(hash_table[i].key == key)
			return ESTATUS_KEY_DUPLICATED;
		if(smld_atomic64_cmpset(&hash_table[i].key, 0, key)){
			smld_atomic64_inc(g_entry_count);
			hash_table[i].value = value;
			return ESTATUS_OK;
		}
	}
	return ESTATUS_TABLE_FULL;

}

/*----------------------------------------------------------------------------*/
ESTATUS smldhash_get64(uint64_t key, uint64_t* value){
	int count = 0;
	for(uint32_t i = integer_hash64(key);count++ < BULK_SIZE; i++){
		i &= BULK_SIZE-1;

		if(hash_table[i].key == key){
			*value = hash_table[i].value;
			return ESTATUS_OK;
		}
	}
	return ESTATUS_KEY_MISSED;
}

/*----------------------------------------------------------------------------*/
ESTATUS smldhash_remove64(uint64_t key){
	if(g_entry_count==NULL||hash_table==NULL)
		return ESTATUS_NOT_INITIALIZED;
	int count = 0;
	for(int i = integer_hash64(key); count++ < BULK_SIZE ;i++){
		i &= BULK_SIZE-1;

		// must be atomic load, x86_64 by default atomic for u64
		uint64_t old = hash_table[i].value;
		if(smld_atomic64_cmpset(&hash_table[i].key, key, 0)){
			//if not same as old value, then it's updated.
			smld_atomic64_cmpset(&hash_table[i].value, old, 0);
			smld_atomic64_dec(g_entry_count);
			return ESTATUS_OK;
		}
	}
	return ESTATUS_KEY_MISSED;
}

/*----------------------------------------------------------------------------*/
uint64_t smldhash_getcount()
{
	if(g_entry_count==NULL||hash_table==NULL)
		return 0;
	return smld_atomic64_read(g_entry_count);

}

//lock, user should specify one lock for it
/*----------------------------------------------------------------------------*/
void smldhash_getlist(void* ptr)
{
	if(g_entry_count==NULL||hash_table==NULL)
		return ;
	uint64_t* tmp = (uint64_t*)ptr;
	uint32_t count = 0;
	uint64_t total = smld_atomic64_read(g_entry_count);
	for(int i = 0; i < BULK_SIZE&&count<=total; i++)
		if(hash_table[i].key != 0){
			 tmp[count++] = hash_table[i].key;
		}
}

//lock, user should specify one lock for it
/*----------------------------------------------------------------------------*/
void smldhash_cleanup()
{
	memset(g_entry_count, 0, SMLDHASH_SHM_SIZE);
}

