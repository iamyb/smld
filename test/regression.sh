#!/bin/bash

echo "---TEST SMLD --------------------------------------"
LD_PRELOAD=../lib/libsmldhook.so ./test_smld.out
echo ""
echo "---TEST SMLD CPP-----------------------------------"
LD_PRELOAD=../lib/libsmldhook.so ./test_smldcpp.out
echo ""
