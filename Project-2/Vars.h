#ifndef VARS_H_
#define VARS_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct PCB
{
    struct PCB *Next_PCB;
    int PID;                      // process ID (0,1,2,3, etc.)
    short int PC, P0, P1, P2, P3; // program counter and pointer registers
    int IC;                       // number of instructions before preemption (i.e., time slice)
    int R0, R1, R2, R3;           // general purpose registers
    char PSW[2];                  // process status word
    int ACC;                      // accumulator
    int BaseReg, LimitReg;        // minimum and maximum memory locations for a process
};

struct Semaphore
{
    int count;
    struct PCB *SemQ;
};

/* These are variables representing the VM itself */

// Pointers to head + tail of ready queue as well as current process
struct PCB *RQ, *RQT, *Current, *tmp;

char IR[6];         // instruction register
short int PC;       // program counter
short int PRegs[4]; // array for pointer registers used by VM
short int RRegs[4]; // array for general registers used by VM

// Pointer registers
short int P0;
short int P1;
short int P2;
short int P3;

// General-Purpose registers
int R0;
int R1;
int R2;
int R3;

int BaseRegister, LimitRegister;

int ACC;              // accumulator
char PSW[2];          // process status word
char memory[1000][6]; // memory where programs and data will be stored
short int opcode;

#endif
