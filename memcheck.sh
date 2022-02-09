#!/bin/bash
# Copyright (c) 2022 Asaf Zebulon
# 
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT


printf "File to test: $1\n"
printf "==================================================\n"
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         $1 