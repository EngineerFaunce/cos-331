## Project Description
The purpose of this project is to simulate the execution of multiple processes
inside of an operating system. It uses a simple Round Robin scheduling algorithm
and each process is given a time-slice, which is the number of instructions it
can execute before giving control back to the OS. The time-slice is implemented
as an integer named IC (Instruction Counter).

The OS maintains a Ready Queue (RQ), which is a linked list of Process Control
Blocks (PCBs) waiting to be given control of the CPU and begin execution. If a
process completes its time-slice, it will be moved to the tail of the RQ and 
the new head of the RQ will execute next. If a process completes its execution
it will be removed from the RQ.

This project is able to load in up to 10 PB12 programs. Each program is given
a unique process ID (PID) and 100 memory locations in the VM. Process 0 occupies
memory locations 0-99, process 1 occupies memory locations 100-199, and so on. 
To ensure that each process stays within its address space, the project 
implements a Base and Limit register. The Base register holds the minimum legal
address and the Limit register holds the maximum legal address space.

## Compiling
This program can be compiled by typing `make` into the terminal and then typing
`./Project1`. If the user wishes to delete compiled files then they can run
`make clean` to remove such files from the root.