#ifndef _system_h_
#define _system_h_

typedef void interrupt (*ptrInter)(...);

class Thread;

class System{
public:
  System();
  ~System();

  //***za zakljucavanje sistema***
  static int unlockedVal;
  static void unlock();
  static void lock();
  static int isUnlocked();
  //*****************************

  //***za pokretanje/zavrsavanje sistema***
  static Thread* main, * idle;
  static void inic();
  static void restore();
  //***************************************

  //***za prekidnu rutinu***
  static int explicit;
  static int count; //brojac 
  static ptrInter oldRoutine;
  static void interrupt timer(...);
  //************************
};

#endif
