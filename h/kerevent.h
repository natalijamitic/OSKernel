#ifndef _kerevent_h_
#define _kerevent_h_

class PCB;
typedef unsigned char IVTNo;

class KernelEv{
public:
	  void signal();
private:
  KernelEv();
  ~KernelEv();

  friend class Event;
  friend class IVTEntry;

  void wait();

  void block();
  void deblock();

  static KernelEv* getKernelEv(IVTNo ivtNo);

  int value;
  PCB* blocked;
};


#endif
