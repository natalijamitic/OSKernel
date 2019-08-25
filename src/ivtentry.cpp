#include "ivtentry.h"
#include "kerevent.h"
#include <dos.h>
#include "system.h"
IVTEntry* IVTEntry::ivt[256];

IVTEntry::IVTEntry(IVTNo ivtNo, ptrInter newRoutine) : ivtEntryNo(ivtNo) {
	oldRoutine = getvect(ivtEntryNo);
	setvect(ivtEntryNo, newRoutine);
	kernelEv = new KernelEv();
	ivt[ivtEntryNo] = this;
}

IVTEntry::~IVTEntry() {
	System::lock();
	//callOldRoutine(); //opciono
	setvect(ivtEntryNo, oldRoutine);
	oldRoutine = 0;
	delete kernelEv;
	kernelEv = 0;
	ivt[ivtEntryNo] = 0;
	System::unlock();
}

void IVTEntry::callOldRoutine() { oldRoutine(); }

KernelEv* IVTEntry::getKernelEv() { return kernelEv; }

KernelEv* IVTEntry::getKernelEv(IVTNo ivtNo) { return ivt[ivtNo]->kernelEv; }
