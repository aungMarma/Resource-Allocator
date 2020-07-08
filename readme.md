What the program does:
This program prompts the user for a command infinitely, unless user command is QUIT.
If the user requests a process to be allocated to the memory , it applies the best-fit strategy
to allocate the process if there are enough spaces. If there are not enough spaces, it does tell the user.
So the user can release an existing process to allocate the new process or QUIT. The best-fit
strategy is to find the smallest hole which is big enough for the new process that to be allocated.
The hole means free block.

When a process gets released, thus creating a hole, if there exits another one hole or two holes (above and below)
next to the existing process, the program merges these holes into one. 

Compaction moves all processes to the end ( or bottom) of the memory, thus freeing up memory at the top. And all free blocks
at the top are merged into one free block.

I used LinkedList as data structure because it is very flexible in terms of merging, adding, removing.

To compile:
gcc -o allocator_AMarma allocator_AMarma.c 
To run:
./allocator_AMarma 1048576

The input and output sample of the program are following:
allocator>RQ P0 262144 B
allocator>RQ P1 262144 B
allocator>RQ P2 262200 B
allocator>STAT
Addresses [0 : 262143] Process P0
Addresses [262144 : 524287] Process P1
Addresses [524288 : 786487] Process P2
Addresses [786488 : 1048575] Free
allocator>RL P1
allocator>RQ P3 200000 B
allocator>STAT
Addresses [0 : 262143] Process P0
Addresses [262144 : 524287] Free
Addresses [524288 : 786487] Process P2
Addresses [786488 : 986487] Process P3
Addresses [986488 : 1048575] Free
allocator>RQ P4 200000 B 
allocator>STAT
Addresses [0 : 262143] Process P0
Addresses [262144 : 462143] Process P4
Addresses [462144 : 524287] Free
Addresses [524288 : 786487] Process P2
Addresses [786488 : 986487] Process P3
Addresses [986488 : 1048575] Free
allocator>C
allocator>STAT
Addresses [0 : 124231] Free
Addresses [124232 : 324231] Process P3
Addresses [324232 : 586431] Process P2
Addresses [586432 : 786431] Process P4
Addresses [786432 : 1048575] Process P0
allocator>QUIT