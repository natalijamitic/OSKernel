#include "sighandl.h"
#include "pcb.h"
#include "system.h"
#include "IOSTREAM.H"

SignalHandlerList::Element::Element(SignalHandler d) {
  data = d;
  next = 0;
}

SignalHandlerList::SignalHandlerList() : first(0), last(0) {}

SignalHandlerList::~SignalHandlerList() {
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

void SignalHandlerList::add(SignalHandler d) {
  System::lock();
	Element* elem = new Element(d);
	if (first == 0)
		first = elem;
	else
		last->next = elem;
	last = elem;
  System::unlock();
}


void SignalHandlerList::swap(SignalHandler hand1, SignalHandler hand2){
	if (hand1 == hand2)
		return;

	System::lock();
	Element* fir = 0, *sec = 0, *cur = first;
	while (cur){
		if (fir && sec)
			break;
		if (cur->data == hand1)
			fir = cur;
		else if (cur->data == hand2)
			sec = cur;

		cur = cur->next;
	}
	if (fir == 0 || sec == 0){
		System::unlock();
		return;
	}

	SignalHandler temp = fir->data;
	fir->data = sec->data;
	sec->data = temp;

	System::unlock();
}

void SignalHandlerList::copy(SignalHandlerList* sig){
	System::lock();
	Element* cur = sig->first;
	while (cur){
		this->add(cur->data);
		cur = cur->next;
	}
	System::unlock();
}

void SignalHandlerList::print(){
	System::lock();
	Element* cur = first;
	while (cur){
		cout << cur->data << " ";
		cur = cur->next;
	}
	System::unlock();
}
