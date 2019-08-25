#include "kersem.h"
#include "semaphor.h"
#include "SCHEDULE.H"
#include "system.h"
#include "pcb.h"
#include <iostream.h>
#include "list.h"
#include "semlist.h"
#include "idle.h"

SemList* KernelSem::semaphoresList = new SemList();
int KernelSem::semaphoreId = 0;

KernelSem::KernelSem(int init):value(init){
  blockedThreadsList = new List();
  timeBlockedThreadsList = new List();
  KernelSem::semaphoresList->add(this);
}

KernelSem::~KernelSem(){
  System::lock();
  KernelSem::semaphoresList->remove(this);
  for(List::Element* cur = blockedThreadsList->getFirst(); cur != 0; cur = cur->next){
	  if (cur->data->getState() != DONE){
		  cur->data->setState(READY);
		  Scheduler::put(cur->data);
	  }
  }
  for(List::Element* cur2 = timeBlockedThreadsList->getFirst(); cur2 != 0; cur2 = cur2->next){
  	  if (cur2->data->getState() != DONE){
  		  cur2->data->setState(READY);
  		  Scheduler::put(cur2->data);
  	 }
   }
  delete blockedThreadsList;
  delete timeBlockedThreadsList;
  value = 0;
  System::unlock();
}

int KernelSem::val() const{
  return value;
}

void KernelSem::deblock(int timeDeblock){
  System::lock();
  PCB* deblockedThread;
  if (timeDeblock == 1){
	  deblockedThread = timeBlockedThreadsList->removeGetFirst();
	  deblockedThread->setTimeUnblocked(1);
  }
  else{
	  PCB* normal = blockedThreadsList->getFirstPCB();
	  PCB* time = timeBlockedThreadsList->getByLowestSemIDPCB();
	  if (normal == 0 && time == 0)
		  deblockedThread = 0;
	  else if (normal == 0 && time != 0)
		  deblockedThread = timeBlockedThreadsList->removeGetLowestSemIDAndUpdateTimeSem(time);
	  else if (normal != 0 && time == 0)
		  deblockedThread = blockedThreadsList->removeGetFirst();
	  else if (normal != 0 && time != 0)
		  deblockedThread = (normal->getMySemaphoreId() < time->getMySemaphoreId() ? blockedThreadsList->removeGetFirst() : timeBlockedThreadsList->removeGetLowestSemIDAndUpdateTimeSem(time));

  }

  if (deblockedThread != 0){
    if (deblockedThread->getState() != DONE) {
				deblockedThread->setState(READY);
				Scheduler::put(deblockedThread);
			}
  }
  System::unlock();
}

void KernelSem::block(Time maxTimeToWait){
  System::lock();
  PCB::running->setState(BLOCKED);
  PCB::running->setMySemaphoreId(KernelSem::semaphoreId++); //da bi se znao redosled kojim se ubacuje u listu cekanja (da se zna sta prvo da se odblokira)
  if (maxTimeToWait == 0)
    blockedThreadsList->add(PCB::running);
  else
    timeBlockedThreadsList->addByTime(PCB::running, maxTimeToWait);
  System::unlock();
}

int KernelSem::wait(Time maxTimeToWait){
  System::lock();
  int ret = 1;
  if (--value < 0){
    block(maxTimeToWait);
    System::unlock();
    dispatch();
    System::lock();
    if (PCB::running->getTimeUnblocked() == 1){
    	ret = 0;
    	PCB::running->setTimeUnblocked(0);//reset flaga
    }
  }
  System::unlock();
  return ret;
}

int KernelSem::signal(int n, int timeDeblock){
  System::lock();

  if (timeDeblock == 1){
	  value++;
	  deblock(timeDeblock);
	  System::unlock();
	  return n;
  }

  else if (n > 0) {
    int deblocked = 0;
    if (value < 0){
      deblocked = ( (n < -1 * value) ? n : -1 * value );
      for (int i = 0; i < deblocked; i++)
        deblock();
    }
    value += n;
    System::unlock();
    return deblocked;
  }

  else if (n == 0){
    if (value++ < 0)
      deblock();
  }
  System::unlock();
  return n;
}

List* KernelSem::getBlockedThreadsList(){ return blockedThreadsList; }
List* KernelSem::getTimeBlockedThreadsList(){ return timeBlockedThreadsList; }
SemList* KernelSem::getSemaphoresList(){ return KernelSem::semaphoresList; }
void KernelSem::deleteSemaphoresList(){
  if (KernelSem::semaphoresList != 0)
    delete KernelSem::semaphoresList;
  KernelSem::semaphoresList = 0;
}

void KernelSem::updateTimeOnSemaphores(){
	if (semaphoresList != 0){
		SemList::updateTime(semaphoresList);
	}

}
