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
#ifndef _SMLD_HOOK_H_
#define _SMLD_HOOK_H_

#ifdef __cplusplus
extern "C"{
#endif

#define SMLDHOOK_SHM_SIZE 100
#define SMLDHOOK_SHM_NAME "/smld_hook"

static volatile int* smld_enabled = NULL;

#ifdef __cplusplus
}
#endif
#endif

