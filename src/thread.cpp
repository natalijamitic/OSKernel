#include "thread.h"
#include <iostream.h>
#include "SCHEDULE.h"
#include "pcb.h"
#include "list.h"
#include "system.h"
#include "idle.h"

Thread::Thread(int i){
  myPCB = new PCB(this);
  start();
}

Thread::Thread (StackSize stackSize, Time timeSlice){
  myPCB = new PCB(this, stackSize, timeSlice);
}

Thread::~Thread(){
  waitToComplete();
  if (myPCB != 0)
    delete myPCB;
  myPCB = 0;
}

void Thread::start(){

  if (myPCB->getState() != INIT) //bez efekta ako se vise puta pozove za istu nit
    return;

  if (myPCB->getId() != myPCB->mainId)
	  myPCB->initStack();

  System::lock();
  myPCB->setState(READY);
  if (myPCB->getId() != PCB::idleId){ //idle nikad u scheduler
	  Scheduler::put(myPCB);
  }
  System::unlock();

}

ID Thread::getId(){
  return myPCB->getId();
}

ID Thread::getRunningId () { return PCB::running->getId(); }

Thread* Thread::getThreadById(ID id) {
  	PCB* pcb = PCB::PCBList->findById(id);
  	return pcb->getThread();
}

void Thread::waitToComplete(){
	if (myPCB->getState() != DONE && PCB::running != this->myPCB && PCB::running != Idle::getIdlePCB()){
		System::lock();
		PCB::running->setState(BLOCKED);
		myPCB->getPCBListWaiting()->add(PCB::running);
		System::unlock();
		dispatch();
	}
}

//***********S I G N A L S**************

ID Thread::getParentId(){
	return myPCB->getParentId();
}

void Thread::signal(SignalId signal){
	if (signal < MAX_SIGNAL && signal != 1 && signal != 2){
		myPCB->mySignals->add(signal);
	}

}

void Thread::signalSend(SignalId signal){ //programer zove, nece korisnik zvati
	myPCB->mySignals->add(signal);
}

void Thread::registerHandler(SignalId signal, SignalHandler handler){
	if (signal < MAX_SIGNAL)
		myPCB->myHandlers[signal]->add(handler);
}

void Thread::unregisterAllHandlers(SignalId id){
	if (id < MAX_SIGNAL){
		if (myPCB->myHandlers[id] != 0){
			delete myPCB->myHandlers[id];
			myPCB->myHandlers[id] = new SignalHandlerList(); //da bude spremno za nova ubacivanje
		}
	}
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
	if (id < MAX_SIGNAL)
		myPCB->myHandlers[id]->swap(hand1, hand2);

}

void Thread::blockSignal(SignalId signal){
	if (signal < MAX_SIGNAL)
		myPCB->blocked[signal] = YES;
}

void Thread::blockSignalGlobally(SignalId signal){
	if (signal < MAX_SIGNAL)
		PCB::blockedGlobally[signal] = YES;
}

void Thread::unblockSignal(SignalId signal){
	if (signal < MAX_SIGNAL)
		myPCB->blocked[signal] = NO;
}

void Thread::unblockSignalGlobally(SignalId signal){
	if (signal < MAX_SIGNAL)
		PCB::blockedGlobally[signal] = NO;
}

//*****************************************

void dispatch (){
  System::explicit = 1;

  asm int 8h; //zove se timer rutina
}
