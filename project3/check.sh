#!/bin/bash
gcc -o test_directory_entries test_directory_entries.c directory_entries.c
./test_directory_entries
rm test_directory_entries
