#!/bin/bash

############################
# TEST DIRECTORY_ENTRIES.C #
############################
gcc -o test_directory_entries test_directory_entries.c directory_entries.c equations.c boot_sector.c fat_entries.c
./test_directory_entries fat32.img
rm test_directory_entries

######################
# TEST BOOT_SECTOR.C #
######################
#gcc -o test_boot_sector test_boot_sector.c equations.c boot_sector.c
#./test_boot_sector fat32.img
#rm test_boot_sector

####################
# TEST EQUATIONS.C #
####################
#gcc -o test_equations test_equations.c equations.c boot_sector.c
#./test_equations fat32.img
#rm test_equations

######################
# TEST FAT_ENTRIES.C #
######################
#gcc -o test_fat_entries test_fat_entries.c fat_entries.c equations.c boot_sector.c
#./test_fat_entries fat32.img
#rm test_fat_entries


