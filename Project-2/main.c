#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Vars.h"
#include "Functions.h"

struct Semaphore
{
    int count ;
    struct PCB *Sem_Queue ;
} ;

struct Semaphore Forks[5] ;
struct Semaphore Doorman ;

// New functions for Project 2
int OS_Trap(char *, struct PCB *) ;
int Wait(struct PCB* , struct Semaphore *) ;
int Signal(struct Semaphore *) ;
int GetPID(struct PCB *) ;

// Previous functions, defined in Function.h to keep main.c neater
extern struct PCB *GetNextProcess(struct PCB **Head);
extern void DeletePCB(struct PCB *Current);
extern void MvToTail (struct PCB *Current, struct PCB **RQT);
extern void PrintQ(struct PCB *Head);
extern void RestoreState(struct PCB *NextProc);
extern void SaveState(struct PCB **PrevProc);
extern void LoadProgram(int PID, struct PCB **tmp);

// Previous functions found in Opcodes.c
extern int ExecuteProc(struct PCB *CurrentProc);
extern int ParseOp1Reg (char *) ;

/* These variables are associated with the implementation of the VM */
int i, j, k ;
int ProgSize ;
char input_line [7] ;

// global pointers for use in RQ
struct PCB *RQ, *tmp, *RQT, *Current ;

int main() {
/*  basically same as before. You are creating 5 philosopher processes (with identical code),
    and creating two separate PB programs: one with the array of fork semaphores and one where
    the doorman is added.

    Make sure you initialize the semaphores!!
*/




while(1)
{
    Current = GetNextProcess(&RQ) ;
    RestoreState(Current) ;
    printf("CURRENT PID %d, IC %d\n", Current->PID, Current->IC) ;
    int Completed = ExecuteProc(Current) ;

    if (Completed == -1)
    {
        printf("Current Process is Blocked on Semaphore.\n") ;
        SaveState(&Current) ;
    }

    if(Completed == 0)
    {
        SaveState(&Current) ;
        printf("Moving PID %d to TAIL\n", Current->PID) ;
        MvToTail(Current, &RQT) ;
        printf("RQT is %d\n", RQT->PID) ;
        if(RQ == NULL)
        RQ = RQT ;
    }

    if (Completed == 1)
    {
        printf("Removing PID %d\n", Current->PID) ;
        DeletePCB(Current) ;
    }

    PrintQ(RQ) ;

    if (RQ == NULL)
        break ;
    }

    return(0) ;
}


//NOTE: Any general purpose registers can be used to hold the system call number and
//OP1 of the Trap Instruction. However, I assume R0 will always be used to hold the system
//call number and R1 will be Op1 of the Trap instruction.

/* New Functions */

/* OS_Trap is called by the process when it executes Opcode 36.
  It returns a value of 1 if the calling process is blocked and 0 if it is
  not blocked. It performs the following actions:

  1) Determines the system call being made.
  2) Calls function Wait, Signal, or GetPID depending on the requested operation. It
      passes the address of the semaphore to be operated on in the case of Wait and
      Signal, and the struct PCB * in case of GetPID.
  3) Returns the value returned by Wait, Signal, or GetPID.
*/

int OS_Trap(char *IR, struct PCB *Current)
{}

/*  Wait performs the basic wait operation on a semaphore.
    Decrements the count variable.

    If count < 0, it places the PCB of the caller on Sema->Sem_Queue, 
    and returns a 1 indicating the process is blocked on the semaphore. 
    
    Otherwise, it returns a 0.
*/
int Wait(struct PCB *Current, struct Semaphore *Sema)
{
    Sema->count--;
    struct PCB *tmp ;
    printf("In Wait with PID %d  Sem.Count = %d\n", Current->PID, Sema->count) ;

    if(Sema->count < 0) {
        printf("Placing process on SemQ.\n");

        /* if SemQ is NULL, then place PCB at head of the SemQ
         * else go to SemQ tail and place PCB there
         */
        if(Sema->SemQ == NULL)
            Sema->SemQ = Current;
        else {
            tmp = Sema->SemQ;
            while(tmp->Next_PCB != NULL)
            tmp = tmp->Next_PCB;
            tmp->Next_PCB = Current;
            tmp->Next_PCB->Next_PCB = NULL;
        }

        return 1;
    }
    else {
    printf("Process was not placed on SemQ.\n");
    return 0;
    }
}

/*  Signal performs the basic signal operation on a semaphore. 
    Increments the count variable.
    If it is <= 0, it picks the PCB from the head of the SemQ and places it on the Ready Queue.
    
    It always returns a 0.
*/ 
int Signal(struct Semaphore *Sema)
{
    Sema->count++;
    struct PCB *tmp ;
    printf("In Signal. Count is %d\n", Sema->count) ;

    if(Sema->count <= 0) {
        tmp = GetNextProcess(&Sema->SemQ);
        printf("Moving process from SemQ to tail of RQ.\n");
        MovetoTail(tmp, &RQT);
    }
}

/*  GetPID places PID of process in Register R1.
    Always returns a 0
*/
int GetPID(struct PCB *Current)
{
    R1 = Current->PID;
    return 0;
}