## What is SMLD 
SMLD(Simple Memory Leak Detector) is a simple implementation to detect memory leak for the allocation using glibc malloc/calloc/realloc/free API. The purpose of SMLD is to provide a RUNTIME memory leak detector solution/demostration for embeded or even lower system which don't have rich debugability and traceability.

The implemenation of SMLD is quite lightweight. It inject several hooks via LD_PRELOAD for memroy allocation/deallocation, to add a WRAPPER upon glibc's related APIs. Those hooks will hash the address of each allocation and unhash it while it's freed. With this, you can observe all kind of memory allocation/deallocation behavior for your program. 

#### Sample Usage
	//hello_smld.c 
	//build: gcc hello_smld.c -o hello_smld.out -L. -lsmld -ldl -lrt
	//run:   LD_PRELOAD=./libsmldhook.so ./hello_smld.out
    #include<stdio.h>
    #include"smld.h"
	void main()
	{
		smld_start();
		void* ptr = (void*)malloc(10);
		printf("%d memory block is leaked...\n", smld_check(NULL);
		smld_stop();
	}

## Supports and Limitations
- Support multiple threads detection.
- Support multiple FORKed processes detection.
- X86_64, LINUX, GCC extension required.
- For C++, only new/delete API are tested. 

## License
>This program is free software; you can redistribute it and/or modify it under
> the terms of the GNU Lesser General Public License as published by the Free
> Software Foundation; either version 3 of the License, or (at your option) any
> later version.

> This program is distributed in the hope that it will be useful,but WITHOUT ANY
> WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
> FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

> You should have received a copy of the GNU Lesser General Public License along
> with this program. If not, see <http://www.gnu.org/licenses/>.
