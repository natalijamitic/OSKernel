#ifndef _list_h_
#define _list_h_

#include "thread.h"
#include "kersem.h"

class PCB;

class List{
private:
  struct Element{
    PCB* data;
    Element* next;
    Time maxTimeToWait; //za semafor
    Element(PCB* d);
    Element(PCB* d, Time t);
    ~Element(){}
  };

  Element* first, * last;
  int count;
  friend class PCB;
public:
  List();
  ~List();

  void add(PCB* d);
  void remove(PCB* d);
  PCB* removeGetFirst();
  PCB* findById(ID id);
  PCB* getByOrder(int n);
  PCB* getFirstPCB();
  PCB* getByLowestSemIDPCB(); //za semafor
  PCB* removeGetLowestSemIDAndUpdateTimeSem(PCB* d); //za semafor
  int getCount();

  Element* getFirst();

  void updateTimeSemaphore(KernelSem* sem); //za semafor
  void addByTime(PCB* d, Time maxTimeToWait); //za semafor

  void print();
};

#endif
