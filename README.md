# file_finder
traverse the files using commands!
commands:
-s <file size>: finds all files with sizes that are equal to less than given arg
-S: prints out the file information
-f <contains substring> <depth>: finds all files that contain the given substring and is in the given depth
<file path>: will traverse given the file path. if no path is given, it will traverse in the current working directy by default

ex: ./a.out -s 1000 -S -f c 0  -> this will traverse all of the files with size lower than 1000, prints out the information, and the files that contain c and is in depth 0.

