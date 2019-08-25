
#include <iostream.h>
#include <dos.h>
#include "SCHEDULE.H"
#include "thread.h"
#include "pcb.h"
#include "idle.h"
#include "list.h"
#include "system.h"
#include <stdarg.h>

extern int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]){
  System* s = new System();

  int ret = userMain(argc, argv);

  cout << ret << endl;

  delete s;
  return ret;
}
