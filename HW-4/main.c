#include <stdio.h>
#include <stdlib.h>

void sleep(int) ;

struct PCB
{   struct PCB *Next_PCB ; 
    int PID ;
    int IC ;
} ;

struct Semaphore 
{   int count ;
    struct PCB *SemQ ;
} ;


/* functions used for assignment */
int Wait(struct Semaphore *, struct PCB *) ;
void Signal (struct Semaphore *) ;
struct PCB *GetNextProcess(struct PCB **) ;
void MovetoTail(struct PCB *Current, struct PCB **RQT) ;
void PrintQ(struct PCB *Head) ;
void Create_PCBs() ;

/*Global Variables*/

struct PCB *RQ, *RQT, *Current ;	// add additional pointers if needed
struct Semaphore Sem ;

int main()
{   
  int blocked ;
  Sem.count = 0 ;
  Sem.SemQ = NULL ;
  Create_PCBs() ; 
  while(1) { 
    Current = GetNextProcess(&RQ) ; //select next process to execute and update RQ
    printf("\n********************\n");
    printf("Current Process is %d\n",Current->PID) ;
    int j = rand() % 4 ; //you will want to play with this!
    if (j < 3 ) {
      printf("Calling WAIT with PID %d\n", Current->PID) ;
      blocked = Wait(&Sem, Current) ;
      if (blocked)
        printf("Process was blocked on wait Q.\n") ;
      else
        printf("Process NOT blocked on wait Q.\n") ;
    }
    else {
      printf("Calling SIGNAL with PID %d\n", Current->PID) ;
      Signal(&Sem) ;	  
      blocked = 0 ;
    }
  
    if (!blocked) {
      MovetoTail(Current, &RQT) ;	
      if (RQ == NULL)
        RQ = RQT ;
    }
    
    printf("Calling PrintQ for the ReadyQ\n") ;
    PrintQ(RQ) ;
    printf("Calling PrintQ for the SemaphoreQ\n") ;
    PrintQ(Sem.SemQ) ;
          
    //sleep(4) ; //Marvel at what you have accomplished!! Can remove once it works.
          
    if (RQ == NULL)
      break ;
  }
}

int Wait(struct Semaphore *Sema, struct PCB *Current)
{
  Sema->count--;
  struct PCB *tmp ;
  printf("In Wait with PID %d  Sem.Count = %d\n", Current->PID, Sema->count) ;
  
  if(Sema->count < 0) {
    printf("Placing process on SemQ.\n");

    /* if SemQ is NULL, then place PCB at head of the SemQ
     * else go to SemQ tail and place PCB there
     * 
     * Could probably be made easier by making a global pointer to SemQ tail
     * and then calling MoveToTail method, but I was too lazy for that.
     * Funny how being lazy involves more work.
     */
    if(Sema->SemQ == NULL) {
      Sema->SemQ = Current;
    }
    else {
      tmp = Sema->SemQ;
      while(tmp->Next_PCB != NULL) {
        tmp = tmp->Next_PCB;
      }
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

void Signal(struct Semaphore *Sema)
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

void Create_PCBs()
{
  struct PCB *tmp ;
  int i ;
	RQ = (struct PCB *) malloc (sizeof (struct PCB)) ;
  RQ->PID = 0;
  tmp = RQ ;
  for(i = 1; i < 10; i++) {
    tmp->Next_PCB = (struct PCB *) malloc (sizeof (struct PCB)) ;
    tmp->Next_PCB->PID = i ;
    tmp->Next_PCB->Next_PCB = NULL ;
    tmp = tmp->Next_PCB ;
  }

  RQT = tmp ;
  RQT->Next_PCB = NULL ;
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
void MovetoTail(struct PCB *Current, struct PCB **RQT)
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
