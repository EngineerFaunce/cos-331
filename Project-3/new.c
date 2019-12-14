#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include "Vars.h"
//#include "Functions.h"

struct PCB *Admit_Program();
void Place_On_Queue(struct PCB *);
void Allocate_Pages(struct Program *);
void Print_Page_Table(int);
void Free_Pages(struct PCB *Current);
void Print_Frames();
int Find_Frame();

void RestoreState(struct PCB *NextProc);
void SaveState(struct PCB **PrevProc);
void DeletePCB(struct PCB *Current);
struct PCB *GetNextProcess(struct PCB **Head);
void MvToTail(struct PCB *Current, struct PCB **RQT);
void PrintQ(struct PCB *Head);

struct Program *New_Allocation;

int NAF = 10;                                               // number of available frames
int Free_Frames[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int On_Disk[10] = {5, 3, 3, 5, 5, 3, 3, 3, 5, 5};           // simulated storage that contains programs of different sizes
int disk_index = 0;                                         // index used to access On_Disk array
int num_requested;                                          // number of pages requested
char temp_memory[21][6];                                    // temp memory (which doesn't seem to be used)
char input[7];                                              // used for reading input from file

/* Global variables used in function implementations */
int fp, i, j, k;

/*
    Admits a program into memory with the following steps:
    - Determines the next program to admit
    - Determines the number of pages the program requires
    - Determines if the program can be brought into memory
        + If it cannot, return NULL
        + Else, allocate page frames, setup processes' page table, copies code
          into into allocated pages, and return a pointer to newly allocated PCB
*/
struct PCB *Admit_Program()
{
    static int First = 1;
    static int PID = 0;
    int Num_Lines;
    if (First)
    {
        First = 0;
        for (i = 0; i < 10; i++)
            for (j = 0; j < 5; j++)
                Page_Table[i][j] = -1;
    }
    if (disk_index >= 10)
    {
        return (NULL);
    }

    num_requested = On_Disk[disk_index];
    printf("Number of Pages Requested is %d\n", num_requested);
    printf("There are %d pages left\n", NAF);
    if (num_requested > NAF)
    {
        printf("NOT ENOUGH PAGES\n");
        return (NULL);
    }
    New_Allocation = (struct Program *)malloc(sizeof(struct Program));
    New_Allocation->New_PCB = (struct PCB *)malloc(sizeof(struct PCB));
    New_Allocation->New_PCB->PID = PID++;
    New_Allocation->New_PCB->PC = 0;
    New_Allocation->New_PCB->ACC = 0;
    New_Allocation->New_PCB->P0 = 0;
    New_Allocation->New_PCB->P1 = 0;
    New_Allocation->New_PCB->P2 = 0;
    New_Allocation->New_PCB->P3 = 0;
    New_Allocation->New_PCB->R3 = 0;
    New_Allocation->New_PCB->R2 = 0;
    New_Allocation->New_PCB->R1 = 0;
    New_Allocation->New_PCB->R0 = 0;
    New_Allocation->New_PCB->Next_PCB = NULL;

    New_Allocation->Num_Lines = 21;

    if (num_requested == 5)
    {
        fp = open("fibonacci.pb", O_RDONLY);
        New_Allocation->Data_Pages = 2;
        New_Allocation->Total_Pages = 5;
        New_Allocation->New_PCB->Total_Pages = 5;
    }

    else
    {
        fp = open("LittleFib.pb", O_RDONLY);
        printf("Opened Little FIB with %d\n", fp);
        New_Allocation->Data_Pages = 0;
        New_Allocation->Total_Pages = 3;
        New_Allocation->New_PCB->Total_Pages = 3;
    }

    Num_Lines = 21;
    for (i = 0; i < Num_Lines; i++)
    {
        k = read(fp, input, 7);
        for (j = 0; j < 6; j++)
        {
            New_Allocation->temp_mem[i][j] = input[j];
        }
    }
    disk_index++;
    Allocate_Pages(New_Allocation);
    return (New_Allocation->New_PCB);
}

// This function takes the PCB returned by Admit_Program() and places it at the tail of the RQ
void Place_On_Queue(struct PCB *Current)
{
    MvToTail(Current, &RQT);
}

// Determines how many pages are required for a program
void Allocate_Pages(struct Program *NP)
{
    int i, j, k, m;
    int PID;
    int Frame;
    int code_pages;
    int num_data, num_needed;
    PID = NP->New_PCB->PID;
    num_needed = NP->Total_Pages;
    code_pages = ceil((double)(NP->Num_Lines / 10.0));
    for (i = 0; i < code_pages; i++)
    {
        Frame = Find_Frame();
        NAF--;
        Page_Table[PID][i] = Frame;
        m = 0;
        for (j = i * 10; j < i * 10 + 10; j++)
        {
            for (k = 0; k < 6; k++)
            {
                memory[Frame * 10 + m][k] = NP->temp_mem[j][k];
            }
            m++;
        }
    }
    num_data = num_needed - code_pages;
    for (i = 0; i < num_data; i++)
    {
        Page_Table[PID][code_pages + i] = Find_Frame();
        NAF--;
    }
}

/* Prints the page table entries for the requested process. */
void Print_Page_Table(int PID)
{
    int i;
    for (i = 0; i < 5; i++)
        printf("Page table %d %d is %d\n", PID, i, Page_Table[PID][i]);
}

/* Frees the pages used by a process when it completes its execution
 * and frees up the pages it has occupied. 
*/
void Free_Pages(struct PCB *Current)
{
    int i, j;
    for (i = 0; i < Current->Total_Pages; i++)
    {
        Free_Frames[Page_Table[Current->PID][i]] = 1;
        Page_Table[Current->PID][i] = -1;
        NAF++;
    }
}

void Print_Frames()
{
    int i;
    for (i = 0; i < 10; i++)
        printf("Frame %d is %d\n", i, Free_Frames[i]);
}

int Find_Frame()
{
    int i;
    for (i = 0; i < 10; i++)
    {
        if (Free_Frames[i] == 1)
        {
            Free_Frames[i] = 0;
            return (i);
        }
    }
    return (-1);
}

/* Old functions */
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
