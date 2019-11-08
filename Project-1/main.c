#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Vars.h"

extern void printMEM(int) ;
void Create_PCBs() ;
void LoadPrograms() ;
void LoadProgram(int, struct PCB **) ;
void RestoreState(struct PCB *) ;
int  ExecuteProc(struct PCB *) ;
void DeletePCB(struct PCB *) ;
void MvToTail(struct PCB *, struct PCB **) ;
void SaveState(struct PCB **) ;
void PrintQ(struct PCB*) ;
struct PCB* GetNextProcess(struct PCB **) ;


/*	The function ExecuteProc is external to this program (simply to keep the main program simple), and 
	executes a process for the number of instructions in its "time-slice", 
	which is held in the IC register of its PCB. It returns a 1 if the process has completed 
	its execution (i.e., it has terminated), or a 0 if it has not yet terminated.
*/
extern int ExecuteProc(struct PCB *) ;

int Max_Line = 0 ;

/*These are variables representing the VM itself*/

int program_line = 0 ; // For loading program into Memory

/*These variables are associated with the implementation of the VM*/
int fp ; 
int i ; 
int j, k ; 
char input_line [7] ;

int main(int argc, char *argv[])
{       Create_PCBs() ;
 	LoadPrograms() ;
        printf("\n***PROGRAMS LOADED. BEGINNING EXECUTION.***\n");
	while(1)
        {
                Current = GetNextProcess(&RQ) ;
	        RestoreState(Current) ;
                Current->IC = (rand() % 200) + 5 ;
                printf("CURRENT PID %d, IC %d\n", Current->PID, Current->IC) ;
                int Completed = ExecuteProc(Current) ;
                if (Completed)
                {
                        printf("Removing PID %d\n", Current->PID) ;
                        DeletePCB(Current) ;
                }
                else
                {
                        SaveState(&Current) ;
                        printf("Process %d has completed time slice. Moving to TAIL\n", Current->PID) ;
                        MvToTail(Current, &RQT) ;
                        printf("RQT is %d\n", RQT->PID) ;
                        if(RQ == NULL)
                                RQ = RQT ;
                }

                PrintQ(RQ) ;
                //sleep(1) ;
                if (RQ == NULL)
                        break ;
        }

        printf("\n-----FINAL MEMORY-----\n");
        /* This is a hard-wired call to printMEM because without it the
           function would only print out the first 40 or so memory locations.
           Also, while each program is correctly stored in memory,the data for 
           each program is not stored in their respective memory location (after
           the program instructions).
        */
	printMEM(1000) ;
}	

void Create_PCBs()
{       RQ = (struct PCB *) malloc (sizeof (struct PCB)) ;
        RQ->PID = 0;
        RQ->IC = (rand() % 200) + 5 ;
        tmp = RQ ;
        for(i = 1; i < 10; i++)
        {       
                tmp->Next_PCB = (struct PCB *) malloc (sizeof (struct PCB)) ;
                tmp->Next_PCB->PID = i ;

                tmp->Next_PCB->IC = (rand() % 200) + 5 ; //rand returns 0 .. MAX
                tmp->Next_PCB->Next_PCB = NULL ;
                tmp = tmp->Next_PCB ;
        }

        RQT = tmp ;
        RQT->Next_PCB = NULL ;

}

void LoadPrograms()
{       struct PCB *tmp ;
  	tmp = RQ ;
  	for (i = 0; i < 10 ; i++)
     	{
      	        LoadProgram(i, &tmp) ;
      		printf("LimitReg = %d. IC = %d\n", tmp->LimitReg, tmp->IC) ;
      		tmp = tmp->Next_PCB ;
     	}
}

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

/*	This function returns the PCB at the head of the RQ and updates
	RQ to point to the next PCB in the list
*/
struct PCB *GetNextProcess(struct PCB **RQ) {
        struct PCB *tmp = *RQ;

        // shifts head of RQ to next PCB
        *RQ = tmp->Next_PCB;
        tmp->Next_PCB = NULL;
        return tmp;
}
       

/*	Deletes the PCB (using free) */
void DeletePCB(struct PCB *Current) {
        Current->Next_PCB = NULL;
        free(Current);
}      

/*	This function places the PCB pointed to by Current at the tail of the
	Ready Queue and updates the RQT pointer.
*/
void MvToTail (struct PCB *Current, struct PCB **RQT) {
        (*RQT)->Next_PCB = Current;
        (*RQT)->Next_PCB->Next_PCB = NULL;
        *RQT = (*RQT)->Next_PCB;
}


/*	Prints out the elements of a linked list */
void PrintQ(struct PCB *Head) {
        while(Head != NULL) {
                printf("Process ID: %d IC: %d \n", Head->PID, Head->IC);
                Head = Head->Next_PCB;
        }
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
