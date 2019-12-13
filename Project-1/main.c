#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Vars.h"
#include "Functions.h"

/*
The function ExecuteProc is external to this program (simply to keep the main program simple), and 
executes a process for the number of instructions in its "time-slice", 
which is held in the IC register of its PCB. It returns a 1 if the process has completed 
its execution (i.e., it has terminated), or a 0 if it has not yet terminated.
*/
extern int ExecuteProc(struct PCB *);
extern void printMEM(int);
extern void printMemRange();

int Max_Line = 0;

int main(int argc, char *argv[])
{
    Create_PCBs();
    LoadPrograms();
    printf("\n***PROGRAMS LOADED. BEGINNING EXECUTION.***\n");
    while (1)
    {
        Current = GetNextProcess(&RQ);
        RestoreState(Current);
        Current->IC = (rand() % 200) + 5;
        printf("CURRENT PID %d, IC %d\n", Current->PID, Current->IC);
        int Completed = ExecuteProc(Current);
        if (Completed)
        {
            printf("\nProcess %d has completed its execution.\n", Current->PID);
            printf("Printing out memory locations utilized by process.\n");
            printMemRange();
            printf("Removing PID %d\n", Current->PID);
            DeletePCB(Current);
            //sleep(2);      // used for demo
        }
        else
        {
            SaveState(&Current);
            printf("Process %d has completed time slice. Moving to TAIL\n", Current->PID);
            MvToTail(Current, &RQT);
            printf("RQT is %d\n", RQT->PID);
            if (RQ == NULL)
                RQ = RQT;
        }

        PrintQ(RQ);
        //sleep(2) ;    // used for demo
        if (RQ == NULL)
            break;
    }

    printf("\n-----FINAL MEMORY-----\n");
    printMEM(Max_Line + 1);
}
