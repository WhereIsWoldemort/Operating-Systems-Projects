#!/bin/bash

gcc -o test_directory_entries test_directory_entries.c directory_entries.c equations.c
./test_directory_entries
rm test_directory_entries

#gcc -o test_boot_sector test_boot_sector.c equations.c boot_sector.c
#./test_boot_sector fat32.img
#rm test_boot_sector
