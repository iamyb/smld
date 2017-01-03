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
#include<iostream>
#include"smld.h"
#include"test_smld.h"

using namespace std;

void testcase_1(void)
{
    int* ptr = new int(1);
    delete ptr;
    int tmp[10];
    int lc = smld_check(tmp);
    EXPECTED("TC1", lc, 0);
}

void testcase_2(void)
{
    int* ptr = new int(1);
    int lc = smld_check(NULL);
    EXPECTED("TC2", lc, 1);
}

int main()
{
    smld_start();
    testcase_1();
    testcase_2();
    smld_stop();
    return 0;
}
