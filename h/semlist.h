#ifndef _semlist_h_
#define _semlist_h_

class KernelSem;

class SemList {
private:
	struct Element {
		KernelSem* data;
		Element* next;
		Element(KernelSem* d);
		~Element() { }
	};

	Element* first, * last;
  int count;
	friend class KernelSem;
	friend class List;
public:
	SemList();
	~SemList();

	void add(KernelSem* d);
	void remove(KernelSem* d);
	KernelSem* removeGetFirst();
	static void updateTime(SemList* semaphoresList);
  int getCount();
};


#endif
