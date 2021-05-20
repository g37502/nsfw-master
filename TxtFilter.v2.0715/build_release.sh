#!/bin/bash
cd TxtFilter/Release
make clean
make
cd -
cd TxtFilterTest/Release
make clean
make
cd -

