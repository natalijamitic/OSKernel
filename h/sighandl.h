#ifndef _sighandl_h_
#define _sighandl_h_

typedef unsigned SignalId;
typedef void (*SignalHandler)();

class SignalHandlerList {
private:
	struct Element {
		SignalHandler data;
		Element* next;
		Element(SignalHandler d);
		~Element() { }
	};


	Element* first, * last;
	friend class PCB;
	friend class SignalList;

public:
	SignalHandlerList();
	~SignalHandlerList();

	void add(SignalHandler d);
	void swap(SignalHandler hand1, SignalHandler hand2);
	void copy(SignalHandlerList* sig);
	void print();
};

#endif
