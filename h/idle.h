#ifndef _idle_h_
#define _idle_h_

#include "thread.h"
#include "pcb.h"

class Idle : public Thread {
public:
  virtual ~Idle();

  static PCB * getIdlePCB();
  static void deleteIdleThread();

protected:
  friend class System;
  Idle();
  virtual void run();

private:
	static volatile int a;
	static Thread* idleThread;
};

#endif
