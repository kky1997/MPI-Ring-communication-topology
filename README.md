# MPI-Ring-communication-topology
Parallel and distributed computing - MPI Ring communication topology

This repository contains PDC A1, an program written in C which utilizes the MPI library to execute message passing between multiple processes.

This program utilizes the ring communication topology, in which data is sent from neighbour to neigbhour, where neighbours are determined by process rank.

Eg. if the program is initilized with 4 processes, they are each given a rank from 0 - 3, where 0 is the highest rank. They now conceptually form a ring where
    process 0's left neighbour is 3 and it's right neighbour is 1.
    process 1's left neighbour is 0 and it's right neighbour is 2.
    Etc.

This program has them send an int value left 3x and then at the end will print, from process p till p - 1 in order of rank, their int "m" value.

Hence, if the starting m-values are their respective rank, eg 0, 1, 2, 3 - after 3 left rotations, their m-values will now be 3, 0, 1, 2.