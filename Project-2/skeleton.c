#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Vars.h"

struct Semaphore
 { int count ;
   struct PCB *Sem_Queue ;
 } ;

struct Semaphore Forks[5] ;
struct Semaphore Doorman ;

//New functions for Project 3

int OS_Trap(char *, struct PCB *) ;
int Wait(struct PCB* , struct Semaphore *) ;
int Signal(struct Semaphore *) ;
int GetPID(struct PCB *) ;

//Previous Functions

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


int main()
{
  /*basically same as before. You are creating 5 philosopher processes (with identical code),
    and creating two separate PB programs: one with the array of fork semaphores and one where the doorman
    Is added.

    Make sure you initialize the semaphores!!
  */

    |
    |
    |
    |
    |

while(1)
          {

                Current = GetNextProcess(&RQ) ;
                RestoreState(Current) ;
                printf("CURRENT PID %d, IC %d\n", Current->PID, Current->IC) ;
                int Completed = ExecuteProc(Current) ;
              
                if (Completed == -1)
                      {printf("Current Process is Blocked on Semaphore.\n”) ;
                       SaveState(&Current) ;
                       }
                
                if(Completed == 0)
                      {SaveState(&Current) ;
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

  /* Performs basic wait operation on the semaphore parameter.
     Decrements the count variable, and, if it is less than 0, places
    the PCB of the caller on Set->Sem_Queue, and returns a 1 indicating
     the process is blocked on the semaphore. Otherwise, it returns a 0.
  
    int Wait(struct PCB *Current, struct Semaphore *Sem)
     {}

  /*Signal performs the basic signal operation on a semaphore. It increments the count
    variable, and, if it is <= 0, it picks the PCB from the head of the semaphore
    Queue and places it on the Ready Queue. It always returns a 0.
  */
   
    int Signal(struct Semaphore *Sem)
     {}
 
  /*GetPID places PID of process in Register R1. While the programmer can specify any
    register, it is a lot simpler to always put it in R1.
     Always returns a 0
  */
     int GetPID(struct PCB *Current)
      {}

  /*Here is the code for instruction 36. Note that it is not processed as a regular instruction
    (That is, there is no OP36() function, the logic is handled as follows:
  */

    case 36: PC++ ; blocked  = OS_Trap(IR, CurrentProc) ;
            if(blocked)
                return(-1) ;
               break ;