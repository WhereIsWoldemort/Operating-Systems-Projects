#!/bin/bash
gcc -o fat32_reader fat32_reader.c 
./fat32_reader fat32.img
