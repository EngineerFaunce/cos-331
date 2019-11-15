#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Vars.h"

struct Semaphore
{
  int count ;
  struct PCB *Sem_Queue ;
} ;

struct Semaphore Forks[5] ;
struct Semaphore Doorman ;

// New functions for Project 3

int OS_Trap(char *, struct PCB *) ;
int Wait(struct PCB* , struct Semaphore *) ;
int Signal(struct Semaphore *) ;
int GetPID(struct PCB *) ;

// Previous Functions

int ParseOp1Reg (char *) ;
struct PCB *GetNextProcess(struct PCB **Head);
void DeletePCB(struct PCB *Current);
void MvToTail (struct PCB *Current, struct PCB **RQT);
void PrintQ(struct PCB *Head);
void RestoreState(struct PCB *NextProc);
void SaveState(struct PCB **PrevProc);
int ExecuteProc(struct PCB *CurrentProc);
void LoadProgram(int PID, struct PCB **tmp);


//Must declare Opcode Functions. Otherwise, end up with a boat load of
//highly annoying compiler warnings!


/*These variables are associated with the implementation of the VM*/
int i, j, k ;
int ProgSize ;
char input_line [7] ;

/*I made these global to make implementation a bit easier.
  You do not have to.
*/

struct PCB *RQ, *tmp, *RQT, *Current ;

int main() {
  /*basically same as before. You are creating 5 philosopher processes (with identical code),
    and creating two separate PB programs: one with the array of fork semaphores and one where the doorman
    Is added.

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

/*  Performs basic wait operation on the semaphore parameter.
    Decrements the count variable, and, if it is less than 0, places
    the PCB of the caller on Set->Sem_Queue, and returns a 1 indicating
    the process is blocked on the semaphore. Otherwise, it returns a 0.
*/
int Wait(struct PCB *Current, struct Semaphore *Sem)
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

/*Signal performs the basic signal operation on a semaphore. It increments the count
  variable, and, if it is <= 0, it picks the PCB from the head of the semaphore
  Queue and places it on the Ready Queue. It always returns a 0.
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

/*  GetPID places PID of process in Register R1. While the programmer can specify any
    register, it is a lot simpler to always put it in R1.
    Always returns a 0
*/
int GetPID(struct PCB *Current)
{
  //
}



/* Old Functions */

void LoadProgram(int PID, struct PCB **tmp)
{
         int i, fp ;
         int program_line = 100 * PID ;
         (*tmp)->BaseReg  = program_line ;
         (*tmp)->LimitReg = program_line + 100;
         fp = open("fibonacci.pb", O_RDONLY) ; //always check the return value.
         printf("Open is %d\n", fp) ;

        if (fp < 0) //error in read
        {       printf("Could not open file\n");
                exit(0) ;
        }

        int ret = read (fp, input_line, 8) ; //returns number of characters read`
        printf("***Number of char read: %d***\n", ret);

        while (1)
        {
                if (ret <= 0) //indicates end of file or error
                        break ; //breaks out of infinite loop

                printf("Copying Program line %d into memory\n", program_line) ;
                for (i = 0; i < 6 ; i++)
                {
                        memory[program_line][i] = input_line[i] ;
                        printf("%c ", memory[program_line][i]) ;
                }
                printf("\n") ;

                ret = read (fp, input_line, 8) ;
                printf("***Number of char read: %d***\n", ret);
                program_line++ ; //now at a new line in the prog
        }

  	printf("Read in Code. Closing File\n") ;
        printf("----------------------------------------------\n") ;
  	close(fp) ;
}

/*	This function restores the state of the process that is set to begin its
	  execution
*/
void RestoreState(struct PCB *NextProc) {
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

        ACC = NextProc->ACC;
        PSW[0] = NextProc->PSW[0];
        PSW[1] = NextProc->PSW[1];

        PC = NextProc->PC;
}

/*	This function saves the state of the VM into the PCB of the process that
	  just completed its "time slice"
*/
void SaveState(struct PCB **PrevProc) {
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

        (*PrevProc)->ACC = ACC;
        (*PrevProc)->PSW[0] = PSW[0];
        (*PrevProc)->PSW[1] = PSW[1];

        (*PrevProc)->PC = PC;
}

/*	Deletes the PCB (using free) */
void DeletePCB(struct PCB *Current) {
        Current->Next_PCB = NULL;
        free(Current);
} 

struct PCB *GetNextProcess(struct PCB **Head)
{
  struct PCB *tmp ;
  tmp = *Head ;
  if (tmp == NULL) {
    printf("No more processes to get fool!\n") ;
    exit(0) ;
  }
  *Head = (*Head)->Next_PCB ;
  tmp->Next_PCB = NULL ;
  return(tmp) ;
}

/*this function takes a pointer to the currently executing process and the address of the Ready
  Queue Tail. It moves the Current process to the tail of the list and updates RQT.
*/            
void MvtoTail(struct PCB *Current, struct PCB **RQT)
{
  (*RQT)->Next_PCB = Current ;
  *RQT = Current ; 
  (*RQT)->Next_PCB = NULL ;
  if (RQ == NULL)
	  RQ = *RQT ;
}

void PrintQ(struct PCB *Head)
{
  struct PCB *tmp ;
  tmp = Head ;
  if (tmp == NULL) {
    printf("Ready Queue is empty!\n") ;
    return ;
  }
  while(tmp != NULL) {
    printf("Process ID %d. \n", tmp->PID) ;
    tmp = tmp->Next_PCB ;
  }
}