#include "list.h"
#include <iostream.h>
#include "pcb.h"
#include "system.h"

List::Element::Element(PCB* d){
  data = d;
  next = 0;

}

List::Element::Element(PCB* d, Time t){
  data = d;
  maxTimeToWait = t;
  next = 0;
}

List::List() : first(0), last(0), count(0){}

List::~List(){
  System::lock();
  while (first != 0){
    Element* old = first;
    first = first->next;
    old->data = 0;
    old->next = 0;
    delete old;
  }
  System::unlock();
}

void List::add(PCB* d){
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

void List::remove(PCB* d){
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

PCB* List::removeGetFirst(){
  if (first == 0)
    return 0;

  Element* old = first;
  PCB* ret = old->data;
  first = first->next;
  if (first == 0)
    last = 0;

  old->data = 0;
  old->next = 0;
  delete old;
  count--;
  return ret;
}

PCB* List::findById(ID id){
  Element* cur = first;
  while (cur != 0){
    if (cur->data->getId() == id)
      return cur->data;
    cur = cur->next;
  }
  return 0;
}

int List::getCount(){
	return count;
}


PCB* List::getByOrder(int n){
	if (n<0 || n>=getCount())
		return 0;

	Element* cur = first;

	for (int i=0; i<n; i++){
		cur = cur->next;
	}

	return cur->data;

}

PCB* List::getFirstPCB(){
	Element* cur = first;
	if (cur == 0)
		return 0;
	else
		return cur->data;

}

PCB* List::getByLowestSemIDPCB(){
	Element* cur = first;
	if (cur == 0)
		return 0;

	int min = cur->data->getMySemaphoreId();
	Element* ret = cur;

	for (cur = first->next; cur != 0; cur = cur->next){
		if (cur->data->getMySemaphoreId() < min){
			min = cur->data->getMySemaphoreId();
			ret = cur;
		}
	}
	return ret->data;

}

PCB* List::removeGetLowestSemIDAndUpdateTimeSem(PCB* d){
	Element* cur = first, * prev = 0;
	  for (; cur != 0 && cur->data != d; prev = cur, cur = cur->next);
	  if (cur == 0)
	    return 0;

	  int addTime = cur->maxTimeToWait;

	  if (prev == 0){ //=remove first
	    first = first->next;
	    first->maxTimeToWait += addTime;
	  } else {
	    prev->next = cur->next;
	    if (cur == last)
	      last = prev;
	    else
	    	cur->next->maxTimeToWait += addTime;
	  }
	  PCB* ret = cur->data;

	  cur->data = 0;
	  cur->next = 0;
	  delete cur;
	  count--;

	  return ret;

}

List::Element* List::getFirst(){
	return first;
}

void List::updateTimeSemaphore(KernelSem* sem){
  System::lock();
  Element* cur = first;
  if (cur == 0){
	  System::unlock();
    return;
  }
  if (cur->maxTimeToWait != 0)
    cur->maxTimeToWait--;

  while (cur->maxTimeToWait == 0){
    cur = cur->next;		//prvo predje na naredni jer ce u sem->signal da se ukloni prvi a ako cur pok ostane na njemu onda se gubi cur pokazivac
    sem->signal(0, 1);
    if (cur == 0){
    	System::unlock();
    	return;
    }
  }
  System::unlock();
}

void List::addByTime(PCB* d, Time maxTimeToWait){
  System::lock();
  Element* cur = first, * prev = 0;
  Element* elem = new Element(d, maxTimeToWait);
  count++;

  if (cur == 0){
    last = first = elem;
    System::unlock();
    return;
  }
  if (elem->maxTimeToWait < first->maxTimeToWait){
    elem->next = first;
    first->maxTimeToWait -= elem->maxTimeToWait;
    first = elem;
    System::unlock();
    return;
  }

  for (; cur != 0; prev = cur, cur = cur->next){
    if (elem->maxTimeToWait > cur->maxTimeToWait)
      elem->maxTimeToWait -= cur->maxTimeToWait;
    else {
      if (elem->maxTimeToWait == cur->maxTimeToWait){
        elem->maxTimeToWait = 0;
        prev = cur;
        cur = cur->next;
      }
      break;
    }
  }

  if (cur == 0){ //na kraj ide
    last = last->next = elem;
    System::unlock();
    return;
  }

  prev->next = elem;
  elem->next = cur;

  cur->maxTimeToWait -= elem->maxTimeToWait;
  System::unlock();
}


void List::print(){
	System::lock();
	cout << "START" << endl;
	for (Element* cur = first; cur!=0; cur=cur->next){
		cout << cur->data->getId() << "  " << cur->maxTimeToWait << endl;
	}

	cout << "DONE" << endl << endl;
	System::unlock();

	for (int i=0; i<5678; i++)
		for (int j=0; j<789; j++);
}
