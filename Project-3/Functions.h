#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Vars.h"
#include "Opcodes.h"

void RestoreState(struct PCB *NextProc);
void SaveState(struct PCB **PrevProc);
void DeletePCB(struct PCB *Current);
struct PCB *GetNextProcess(struct PCB **Head);
void MvToTail(struct PCB *Current, struct PCB **RQT);
void PrintQ(struct PCB *Head);

/* Functions used in main.c */

// Restores the state of the process that is set to begin its execution
void RestoreState(struct PCB *NextProc)
{
    PRegs[0] = NextProc->P0;
    PRegs[1] = NextProc->P1;
    PRegs[2] = NextProc->P2;
    PRegs[3] = NextProc->P3;

    RRegs[0] = NextProc->R0;
    RRegs[1] = NextProc->R1;
    RRegs[2] = NextProc->R2;
    RRegs[3] = NextProc->R3;

    BaseRegister = NextProc->BaseReg;
    LimitRegister = NextProc->LimitReg;
    TotalPages = NextProc->Total_Pages;

    ACC = NextProc->ACC;
    PSW[0] = NextProc->PSW[0];
    PSW[1] = NextProc->PSW[1];

    PC = NextProc->PC;
}

// This function saves the state of the VM into the PCB of the process that just completed its "time slice"
void SaveState(struct PCB **PrevProc)
{
    (*PrevProc)->P0 = PRegs[0];
    (*PrevProc)->P1 = PRegs[1];
    (*PrevProc)->P2 = PRegs[2];
    (*PrevProc)->P3 = PRegs[3];

    (*PrevProc)->R0 = RRegs[0];
    (*PrevProc)->R1 = RRegs[1];
    (*PrevProc)->R2 = RRegs[2];
    (*PrevProc)->R3 = RRegs[3];

    (*PrevProc)->BaseReg = BaseRegister;
    (*PrevProc)->LimitReg = LimitRegister;
    (*PrevProc)->Total_Pages = TotalPages;

    (*PrevProc)->ACC = ACC;
    (*PrevProc)->PSW[0] = PSW[0];
    (*PrevProc)->PSW[1] = PSW[1];

    (*PrevProc)->PC = PC;
}

// Frees the current process
void DeletePCB(struct PCB *Current)
{
    Current->Next_PCB = NULL;
    free(Current);
}

// Removes the process at the head of the Ready Queue and returns a pointer to it
struct PCB *GetNextProcess(struct PCB **Head)
{
    struct PCB *tmp;
    tmp = *Head;
    if (tmp == NULL)
    {
        printf("No more processes to get fool!\n");
        exit(0);
    }
    *Head = (*Head)->Next_PCB;
    tmp->Next_PCB = NULL;
    return (tmp);
}

/*  Takes a pointer to the currently executing process and moves it to the tail of
    the Ready Queue (RQT)
*/
void MvToTail(struct PCB *Current, struct PCB **RQT)
{
    (*RQT)->Next_PCB = Current;
    *RQT = Current;
    (*RQT)->Next_PCB = NULL;
    if (RQ == NULL)
        RQ = *RQT;
}

// Prints out the Ready Queue
void PrintQ(struct PCB *Head)
{
    struct PCB *tmp;
    tmp = Head;
    if (tmp == NULL)
    {
        printf("Ready Queue is empty!\n");
        return;
    }
    while (tmp != NULL)
    {
        printf("Process ID %d. \n", tmp->PID);
        tmp = tmp->Next_PCB;
    }
}

#endif