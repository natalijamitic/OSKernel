#include "kerevent.h"
#include "event.h"
#include "pcb.h"
#include "SCHEDULE.H"
#include "ivtentry.h"
#include "system.h"
#include <iostream.h>


KernelEv::KernelEv() : blocked(0), value(0) {}

KernelEv::~KernelEv() {
	System::lock();
	if (blocked != 0){
		if (blocked->getState() != DONE){
			blocked->setState(READY);
			Scheduler::put(blocked);
	    }
	}
	blocked = 0;
	value = 0;

	System::unlock();
}

void KernelEv::block(){
  System::lock();
  PCB::running->setState(BLOCKED);
  blocked = PCB::running;
  System::unlock();
}

void KernelEv::deblock(){
  System::lock();
  if (blocked->getState() != DONE){
    blocked->setState(READY);
    Scheduler::put(blocked);
  }
  System::unlock();
}

void KernelEv::signal() {
  System::lock();
  if (blocked == 0){
    value = 1;
  }
  else if (blocked != 0){
    deblock();
    blocked = 0;
    value = 0;
  }
  System::unlock();

  dispatch(); //brza obrada
}

void KernelEv::wait() {
  System::lock();
   if (value == 0) {
     block();
     System::unlock();
     dispatch();
   }
   else if (value == 1){
     value = 0;
     System::unlock();
   }
}

KernelEv* KernelEv::getKernelEv(IVTNo ivtNo) { return IVTEntry::getKernelEv(ivtNo); }
