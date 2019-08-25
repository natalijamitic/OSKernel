#include "semlist.h"
#include "kersem.h"
#include "list.h"
#include "system.h"
#include "pcb.h"
#include <stdlib.h>

#include <iostream.h>

SemList::Element::Element(KernelSem* d) {
	data = d;
	next = 0;
}

SemList::SemList() : first(0), last(0), count(0) {}

SemList::~SemList() {
	System::lock();
	while (first != 0) {
		Element* old = first;
		first = first->next;
		old->data = 0;
    old->next = 0;
		delete old;
	}
	System::unlock();
}

void SemList::add(KernelSem* d) {
	System::lock();
	Element* elem = new Element(d);
	if (first == 0)
		first = elem;
	else
		last->next = elem;
	last = elem;
  count++;
  System::unlock();
}

void SemList::remove(KernelSem* d) {
	Element* cur = first, * prev = 0;
  for (; cur != 0 && cur->data != d; prev = cur, cur = cur->next);
  if (cur == 0)
    return;

  if (prev == 0){ //=remove first
    first = first->next;
  } else {
    prev->next = cur->next;
    if (cur == last)
      last = prev;
  }
  cur->data = 0;
  cur->next = 0;
  delete cur;
  count--;
}

KernelSem* SemList::removeGetFirst(){
  if (first == 0)
    return 0;

  Element* old = first;
  KernelSem* ret = old->data;
  first = first->next;
  if (first == 0)
    last = 0;

  old->data = 0;
  old->next = 0;
  delete old;
  count--;
  return ret;
}

void SemList::updateTime(SemList* semaphoresList) {
	System::lock();
	Element* cur = semaphoresList->first;

	while (cur != 0) {
		KernelSem* sem = cur->data;
		//cur->data->getTimeBlockedThreadsList()->print();
		sem->getTimeBlockedThreadsList()->updateTimeSemaphore(sem);
		cur = cur->next;
	}
	System::unlock();
}


int SemList::getCount(){
	return count;
}
