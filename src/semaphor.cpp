#include "semaphor.h"
#include "kersem.h"
#include "list.h"
#include "semlist.h"
#include "system.h"

Semaphore::Semaphore(int init) {
  myImpl = new KernelSem(init);


}

Semaphore::~Semaphore(){
	System::lock();
	delete myImpl;
	System::unlock();
}

int Semaphore::wait(Time maxTimeToWait) {
	int ret = myImpl->wait(maxTimeToWait);

	return ret;
}

int Semaphore::signal(int n) {
	return myImpl->signal(n);
}

int Semaphore::val() const {
	return myImpl->val();
}
