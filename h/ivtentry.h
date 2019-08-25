#ifndef _ivtentry_h_
#define _ivtentry_h_


typedef unsigned char IVTNo;
typedef void interrupt (*ptrInter)(...);
class KernelEv;

class IVTEntry {
public:
	IVTEntry(IVTNo ivtNo, ptrInter newRoutine);
	~IVTEntry();
	void callOldRoutine();
	KernelEv* getKernelEv();

private:

	IVTNo ivtEntryNo;
	ptrInter oldRoutine;
	KernelEv* kernelEv;

	static IVTEntry* ivt[256];
	static KernelEv* getKernelEv(IVTNo ivtNo);

	friend class KernelEv;

};


#define PREPAREENTRY(ivtEntryNo, callOld)\
	void interrupt intrRoutine##ivtEntryNo(...);\
	IVTEntry ivtEntry##ivtEntryNo(ivtEntryNo, intrRoutine##ivtEntryNo);\
	void interrupt intrRoutine##ivtEntryNo(...) {\
		ivtEntry##ivtEntryNo.getKernelEv()->signal();\
		if (callOld == 1)\
	    	ivtEntry##ivtEntryNo.callOldRoutine();\
	}\

#endif
