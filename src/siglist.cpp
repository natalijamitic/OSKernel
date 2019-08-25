#include "siglist.h"
#include "pcb.h"
#include "system.h"
#include "thread.h"
#include "sighandl.h"
#include <IOSTREAM.H>

SignalList::Element::Element(SignalId d) {
  data = d;
  next = 0;
}

SignalList::SignalList() : first(0), last(0) {}

SignalList::~SignalList() {
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

void SignalList::add(SignalId d) {
  System::lock();
	Element* elem = new Element(d);
	if (first == 0)
		first = elem;
	else
		last->next = elem;
	last = elem;
  System::unlock();
}

void SignalList::processSignals(PCB* pcb){
	System::lock();

	Element* cur = first, * prev = 0, * old = 0;

	while (cur){
		int signal = cur->data;

		if (pcb->blocked[signal] == NO && PCB::blockedGlobally[signal] == NO){
			old = cur;
			if (prev == 0){
				cur = first = first->next;
			}
			else {
				prev->next = cur->next;
				if (cur == last){
					last = prev;
					cur = 0;
				}
				else
					cur = cur->next;
			}
			old->next = 0;


			for (SignalHandlerList::Element* temp = pcb->myHandlers[signal]->first; temp != 0; temp = temp->next){
				(*temp->data)();
			}

			delete old;
		}
		else{
			prev = cur;
			cur = cur->next;
		}

	}

	System::unlock();
}
