#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Vars.h"

// Found in new.c
extern struct PCB *Admit_Program();
extern void Place_On_Queue();
extern void Print_Page_Table(int);
extern void Free_Pages(struct PCB *);

extern void RestoreState(struct PCB *NextProc);
extern void SaveState(struct PCB **PrevProc);
extern void DeletePCB(struct PCB *Current);
extern struct PCB *GetNextProcess(struct PCB **Head);
extern void MvToTail(struct PCB *Current, struct PCB **RQT);
extern void PrintQ(struct PCB *Head);

// Found in Opcodes.c
extern int ExecuteProc(struct PCB *);
extern void printMEM(int);

struct PCB *New_Program;

/*These are variables representing the VM itself*/
int Max_Line = 0;
int program_line = 0; // For loading program into Memory

/*These variables are associated with the implementation of the VM*/
int fp, i, j, k;
char input_line[7];

int main(int argc, char *argv[])
{
    RQ = NULL;
    RQT = NULL; /* Changed how PCB allocated and placed on RQ */
    while (1)
    {
        New_Program = Admit_Program();
        if (New_Program == NULL)
            break;
        printf("Putting PID %d on the RQ\n", New_Program->PID);
        Place_On_Queue(New_Program);
    }

    while (1)
    {
        Current = GetNextProcess(&RQ);    //Standard and already coded
        RestoreState(Current);            //Restores the status of the registers, all registers, PSW, ACC, PC, etc
        Current->IC = (rand() % 200) + 5; //Assigns a random time slice for the program that was loaded
        printf("CURRENT PID %d, IC %d\n", Current->PID, Current->IC);
        int Completed = ExecuteProc(Current); // checks to see if the execute function returns a 1 (true)
        if (Completed)
        {
            printf("The program in PCB %d has completed its exeuction and will be terminated\n", Current->PID);
            printf("Removing PID %d\n", Current->PID); // Program has completed execution and will terminate
            //Free_Pages(Current);
            DeletePCB(Current); // Calls DeletePCB, expanded upon below

            while (1)
            {
                New_Program = Admit_Program();
                if (New_Program == NULL)
                    break;
                printf("Putting PID %d on the RQ\n", New_Program->PID);
                printf("PID is %d PC is %d\n", New_Program->PID, New_Program->PC);
                Place_On_Queue(New_Program);
            }
        }
        else
        {
            SaveState(&Current);
            printf("Moving PID %d to TAIL\n", Current->PID); // Moves the PCB back to the end of the queue
            MvToTail(Current, &RQT);
            printf("RQT is %d\n", RQT->PID);
            if (RQ == NULL)
                RQ = RQT;
        }
        PrintQ(RQ); // Prints the state of the ready queue
        sleep(1);
        if (RQ == NULL) // If RQ is NULL this breaks the while loop and we're done
            break;
    }

    printMEM(100); // prints out the final state of the main memory
}