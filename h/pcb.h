#ifndef _pcb_h_
#define _pcb_h_

#include "thread.h"
#include "list.h"
#include "siglist.h"
#include "sighandl.h"

#define MAX_STACK_SIZE 65536 //max za stack
#define MAX_SIGNAL 16

enum State {INIT, RUNNING, BLOCKED, READY, DONE, DELETE}; //dodati po potrebi
enum SignalState {YES, NO}; //za signale

class PCB{
public:
  static PCB* running, *main, *idle;
  static ID mainId, idleId;
  unsigned* stack;
  unsigned ss;
  unsigned sp;
  unsigned bp;

//geteri za private clanove
  StackSize getStackSize();
  Time getTimeSlice();
  State getState();
  ID getId();
  Thread* getThread();
  List* getPCBListWaiting();

  static List* getPCBList();
//

  static void deletePCBList();
  void setState(State state);
  void activatePCBListWaiting();


  int getTimeUnblocked();
  void setTimeUnblocked(int n);
  void setMySemaphoreId(int n);
  int getMySemaphoreId();

  //***********S I G N A L S**************
  void processSignals();
  ID getParentId();

  static void setSignalStateBlockedGlobally(SignalState s);
  //*************************************

protected:
  friend class Thread;
  PCB(Thread* thread, StackSize stackSize, Time timeSlice);
  PCB(Thread* thread); //za main nit
  ~PCB();

  static void wrapper();
  void initStack();

private:
  friend class SignalHandlerList;
  friend class SignalList;
  Thread *myThread;
  StackSize myThreadStackSize;
  Time myThreadTimeSlice;
  State myThreadState;
  ID myThreadId;
  List* PCBListWaiting; //oni koji cekaju da se pcb zavrsi

  int timeUnblocked;
  int mySemaphoreId;

  //***********S I G N A L S**************
  PCB* parent;
  SignalHandlerList* myHandlers[MAX_SIGNAL];
  SignalList* mySignals;
  SignalState blocked[MAX_SIGNAL];

  static SignalState blockedGlobally[MAX_SIGNAL];

  static void signalHandlerZero();
  //*************************************

  static int Id;
  static List* PCBList; //lista svih PCB-ova

};

#endif
