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
#ifndef _SMLD_HASH_H_
#define _SMLD_HASH_H_

#ifdef __cplusplus
extern "C"{
#endif

#include<stdint.h>
#ifndef BULK_SIZE
#define BULK_SIZE (1<<20) 
#endif

typedef enum ESTATUS{
	ESTATUS_OK,
	ESTATUS_TABLE_FULL,
	ESTATUS_KEY_DUPLICATED,
	ESTATUS_KEY_MISSED,
	ESTATUS_NOT_INITIALIZED
}ESTATUS;

ESTATUS     smldhash_put64(uint64_t key, uint64_t  value);
ESTATUS     smldhash_get64(uint64_t key, uint64_t *value);
ESTATUS     smldhash_remove64(uint64_t key);
uint64_t    smldhash_getcount();
void        smldhash_getlist(void* ptr);


#ifdef __cplusplus
}
#endif
#endif



