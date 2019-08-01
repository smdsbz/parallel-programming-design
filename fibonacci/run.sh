#!/bin/bash

gcc -o fib main.c -lm -pthread
/usr/bin/time -v ./fib
rm ./fib