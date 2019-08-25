#ifndef _event_h_
#define _event_h_

#include "ivtentry.h"
#include "kerevent.h"
typedef unsigned char IVTNo;
class KernelEv;
class PCB;

class Event {
public:
	Event(IVTNo ivtNo);
	~Event();

	void wait();

	PCB* getCreator();
protected:
	friend class KernelEv; // can call KernelEv
	void signal();
private:
	KernelEv* myImpl;
	PCB* creator;
};
#endif
