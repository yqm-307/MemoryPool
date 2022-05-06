#!/bin/bash

clear="$*" #获取环境变量

clear()
{
    rm -rf CMakeFiles
    rm *cmake*
    rm CMakeCache.txt
    rm Makefile
    rm -rf bin
    rm log.txt
}

build()
{
    cmake .
    make
}

if [ "${clear}" = "clear" ]
then
    clear
    #cd test
    #./clear.sh
else
    build
fi