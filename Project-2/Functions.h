#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

struct PCB *GetNextProcess(struct PCB **Head);
void DeletePCB(struct PCB *Current);
void MvToTail(struct PCB *Current, struct PCB **RQT);
void PrintQ(struct PCB *Head);
void RestoreState(struct PCB *NextProc);
void SaveState(struct PCB **PrevProc);
void Create_PCBs();
void LoadPrograms(char file[]);
void LoadProgram(int PID, struct PCB **tmp, char file[]);

#endif
