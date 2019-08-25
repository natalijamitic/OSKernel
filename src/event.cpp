#include "event.h"
#include "kerevent.h"
#include "pcb.h"

Event::Event(IVTNo ivtNo) : creator(PCB::running) {
	myImpl = KernelEv::getKernelEv(ivtNo);
}

void Event::wait() {
	if (PCB::running == creator)
		myImpl->wait();
}

void Event::signal() {
	myImpl->signal();
}

Event::~Event() {}

PCB* Event::getCreator() { return creator; }
