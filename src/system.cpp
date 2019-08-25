#include "system.h"
#include "pcb.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <stdlib.h>
#include "idle.h"
#include "thread.h"
#include "list.h"
#include <iostream.h>
#include "kersem.h"

extern void tick();
extern int syncPrintf(const char *format, ...);

Thread* System::main = 0;
Thread* System::idle = 0;

System::System(){
  inic();
  System::main = new Thread(0); //pravi se main nit
  System::idle = new Idle(); //pravi se idle
  PCB::setSignalStateBlockedGlobally(NO);
  //cout << "pocinje" <<endl;

}
System::~System(){
  System::lock();
  restore();
  KernelSem::deleteSemaphoresList();
  Idle::deleteIdleThread();
  PCB::deletePCBList();

  delete main;
 // cout << "~~~~" << endl;
  System::unlock();
}



//***za pokretanje/zavrsavanje sistema***
void System::inic(){
  System::lock();
  oldRoutine = getvect(0x08);
  setvect(0x08, System::timer);
  System::unlock();
}

void System::restore(){
  System::lock();
  setvect(0x08, System::oldRoutine);
  System::unlock();
}
//***************************************



//**za prekindu rutinu***
int System::explicit = 0;
int System::count = 0;
ptrInter System::oldRoutine = 0;

void interrupt System::timer(...){
  static unsigned tsp, tss, tbp;

  if (System::isUnlocked() && (System::explicit || (PCB::running->getTimeSlice() != 0 && PCB::running->getTimeSlice() <= System::count))){
    asm{
      mov tsp, sp
      mov tss, ss
      mov tbp, bp
    }
    PCB::running->sp = tsp;
    PCB::running->ss = tss;
    PCB::running->bp = tbp;

    if (PCB::running->getState() == DELETE){ //brisanje steka, vise ne treba, nit je gotova, nece ni uci u Sched
    	PCB::running->setState(DONE);
    	if (PCB::running->stack != 0){
    		    delete [] PCB::running->stack;
    		    PCB::running->stack = 0;
    		}
    }
    else if (PCB::running->getState() == RUNNING && PCB::running != Idle::getIdlePCB()){
      PCB::running->setState(READY);
      Scheduler::put(PCB::running);
    }
    PCB::running = Scheduler::get();

    if (PCB::running == 0 || PCB::running->getState() == DONE){    //|| PCB::running->getState() == BLOCKED
      PCB::running = Idle::getIdlePCB();
    }

    PCB::running->setState(RUNNING);


    tsp = PCB::running->sp;
    tss = PCB::running->ss;
    tbp = PCB::running->bp;

    asm{
      mov sp, tsp
      mov ss, tss
      mov bp, tbp
    }

    System::count = 0;

    System::lock(); //za svaki slucaj
    PCB::running->processSignals();
    System::unlock();

  }
  if (System::explicit == 0){

    System::count++;
    KernelSem::updateTimeOnSemaphores();
    System::oldRoutine();
    tick();
  }
  if (System::explicit == 1 && System::isUnlocked()){
    System::explicit = 0;
  }
}
//***********************



//***za zakljucavanje sistema***
int System::unlockedVal = 1;

void System::unlock(){
  if (unlockedVal == 1)
    return;
  unlockedVal++;
}

void System::lock(){
  unlockedVal--;
}

int System::isUnlocked(){
	if (unlockedVal == 1)
		return 1;
	return 0;
}
//******************************
