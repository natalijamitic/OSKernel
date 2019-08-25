#include "pcb.h"
#include <dos.h>
#include <iostream.h>
#include "SCHEDULE.H"
#include "thread.h"
#include "list.h"
#include "system.h"
#include "siglist.h"
#include "sighandl.h"

PCB* PCB::running = 0;
PCB* PCB::main = 0;
PCB* PCB::idle = 0;
List* PCB::PCBList = new List();
int PCB::Id = 0;
int PCB::idleId = 0;
int PCB::mainId = 0;

SignalState PCB::blockedGlobally[MAX_SIGNAL];

PCB::PCB(Thread* thread, StackSize stackSize, Time timeSlice) : myThread(thread), myThreadTimeSlice(timeSlice), timeUnblocked(0){
  myThreadState = INIT;
  stack = 0; //tek kad se startuje nit to cemo da pravimo da ne trosimo bzvz prostor
  myThreadId = PCB::Id++;
  if (stackSize > MAX_STACK_SIZE)
    stackSize = MAX_STACK_SIZE;
  myThreadStackSize = stackSize >> 1; //ovo je zbog unsigned koji je 2B (16bit) a StackSize je u 1B
  PCBListWaiting = new List();
  PCBList->add(this);

  parent = PCB::running;
  mySignals = new SignalList();

  for (int i = 0; i < MAX_SIGNAL; i++){
	  blocked[i] = parent->blocked[i];
	  myHandlers[i] = new SignalHandlerList();
	  myHandlers[i]->copy(parent->myHandlers[i]); //preuzimaju se hendleri
  }

 // myHandlers[0]->add(signalHandlerZero); //ako se preuzimaju hendleri ovo je suvisno
}

PCB::PCB(Thread* thread) : myThread(thread), myThreadTimeSlice(1), myThreadState(INIT), timeUnblocked(0){
  stack = 0;
  myThreadId = PCB::Id++;
  PCB::mainId = myThreadId;
  PCB::running = this;
  PCB::main = this;
  PCBList->add(this);

  parent = 0;
  mySignals = new SignalList();

  for (int i = 0; i < MAX_SIGNAL; i++){
	  blocked[i] = NO;
	  myHandlers[i] = new SignalHandlerList();
   }

  myHandlers[0]->add(signalHandlerZero);
}

PCB::~PCB(){
  System::lock();
  myThread->myPCB = 0;
  PCB::getPCBList()->remove(this);
  System::unlock();
}

void PCB::initStack(){
  stack = new unsigned[myThreadStackSize];
  stack[myThreadStackSize - 1] = 0x200; //psw::I=1
  stack[myThreadStackSize - 2] = FP_SEG(PCB::wrapper); //podmece se wrapper
  stack[myThreadStackSize - 3] = FP_OFF(PCB::wrapper);
  //od -4 do -11 idu ostali registri koji postoje
  ss = FP_SEG(stack + myThreadStackSize - 12);
  sp = FP_OFF(stack + myThreadStackSize - 12);
  bp = sp;

}

void PCB::setState(State state){
  myThreadState = state;
}

void PCB::activatePCBListWaiting(){
  List* list = PCBListWaiting;
  if (list == 0)
    return;

  System::lock();
  while(list->getCount()){
    PCB* cur = list->removeGetFirst();
    if (cur != 0){
      if (cur->getState() != DONE){
        cur->setState(READY);
        Scheduler::put(cur);
      }
    }
  }
  System::unlock();
}

StackSize PCB::getStackSize(){ return myThreadStackSize; }
Time PCB::getTimeSlice(){ return myThreadTimeSlice; }
State PCB::getState(){ return myThreadState; }
ID PCB::getId(){ return myThreadId; }
Thread* PCB::getThread(){ return myThread; }
List* PCB::getPCBListWaiting(){ return PCBListWaiting; }
List* PCB::getPCBList(){ return PCBList; }
void PCB::deletePCBList(){
  delete PCBList;
}

void PCB::wrapper(){
  PCB::running->myThread->run();

  System::lock();

  if (PCB::running->parent != 0)
	PCB::running->parent->myThread->signalSend(1);

  PCB::running->myThread->signalSend(2);

  PCB::running->processSignals(); //da li odmah preci na brisanje ili ipak obraditi signale ovde


  PCB::running->myThreadState = DELETE; //DONE
  PCB::running->activatePCBListWaiting(); //aktiviraju se svi koji su cekali da bas ova nit zavrsi

  /*
  if (PCB::running->stack != 0){
      		    delete [] PCB::running->stack;
      		    PCB::running->stack = 0;
   }
  */
  if (PCB::running->PCBListWaiting != 0) {
    delete PCB::running->PCBListWaiting;
    PCB::running->PCBListWaiting = 0;
  }
  if (PCB::running->mySignals != 0){
  	delete PCB::running->mySignals;
  	PCB::running->mySignals = 0;
  }
  for (int i = 0; i < MAX_SIGNAL; i++){
  	if (PCB::running->myHandlers[i] != 0)
  		delete PCB::running->myHandlers[i];
  }

  System::unlock();
  while (!System::isUnlocked())
  		System::unlock();
  dispatch();
}


int PCB::getTimeUnblocked(){return timeUnblocked;}
void PCB::setTimeUnblocked(int n){
	  timeUnblocked = n;
}
int PCB::getMySemaphoreId(){return mySemaphoreId;}
void PCB::setMySemaphoreId(int n){
	mySemaphoreId = n;
}


//************S I G N A L I********************

void PCB::processSignals(){
	mySignals->processSignals(this);
}

ID PCB::getParentId(){
	return parent->getId();
}

void PCB::setSignalStateBlockedGlobally(SignalState s){
	for (int i = 0; i < MAX_SIGNAL; i++)
		PCB::blockedGlobally[i] = s;
}

void PCB::signalHandlerZero(){
	System::lock();


	if (PCB::running->parent != 0)
		PCB::running->parent->myThread->signalSend(1);

	PCB::running->myThreadState = DELETE; //DONE
	PCB::running->myThread->signalSend(2); //zapravo suvisno jer se nece ni obraditi

	//PCB::running->processSignals();//ne treba jer mora prvo/odmah kill da se obradi a ne neki drugi signali


	PCB::running->activatePCBListWaiting();


	/*
	  if (PCB::running->stack != 0){
	      		    delete [] PCB::running->stack;
	      		    PCB::running->stack = 0;
	   }
	  */
	if (PCB::running->PCBListWaiting != 0) {
	   delete PCB::running->PCBListWaiting;
	   PCB::running->PCBListWaiting = 0;
	}
	if (PCB::running->mySignals != 0){
		delete PCB::running->mySignals;
		PCB::running->mySignals = 0;
	}
	for (int i = 0; i < MAX_SIGNAL; i++){
		if (PCB::running->myHandlers[i] != 0){
			delete PCB::running->myHandlers[i];
			PCB::running->myHandlers[i] = 0;
		}
	}

	System::unlock();
	while (!System::isUnlocked())
		System::unlock();

	dispatch();

}
