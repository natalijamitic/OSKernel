#include "idle.h"
#include <iostream.h>

Thread* Idle::idleThread = 0;
volatile int Idle::a = 1;

Idle::Idle() : Thread(4096, 1){ //1 za tajmslajs, svaki put da se proveri da l moze da se menja kontekst
  if (idleThread == 0){
    PCB::idleId = getId();
	Idle::idleThread = this;
    PCB::idle = this->myPCB;
	this->start();
  }
}

Idle::~Idle(){}

void Idle::run(){
	while (a){
		a = 1;
	}
}

PCB * Idle::getIdlePCB() { return idleThread->myPCB; } //return PCB::idle;


void Idle::deleteIdleThread() {
	if (idleThread != 0){
		delete idleThread;
		idleThread = 0;
		PCB::idle = 0;
	}
}

