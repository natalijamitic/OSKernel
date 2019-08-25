/*
 * siglist.h
 *
 *  Created on: May 28, 2019
 *      Author: OS1
 */

#ifndef _siglist_h_
#define _siglist_h_

typedef unsigned SignalId;
typedef void (*SignalHandler)();

class SignalList {
private:
	struct Element {
		SignalId data;
		Element* next;
		Element(SignalId d);
		~Element() { }
	};


	Element* first, * last;
	friend class PCB;

	friend class SignalHandlerList;
	friend class PCB;

public:
	SignalList();
	~SignalList();

	void add(SignalId d);
	void processSignals(PCB* pcb);
};




#endif /* SIGLIST_H_ */
