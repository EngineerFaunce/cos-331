#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#include "Vars.h"
#include "Opcodes.h"

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

#endif