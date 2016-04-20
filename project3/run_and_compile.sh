#!/bin/bash
gcc -o fat32_reader fat32_reader.c directory_entries.c equations.c boot_sector.c
./fat32_reader fat32.img
