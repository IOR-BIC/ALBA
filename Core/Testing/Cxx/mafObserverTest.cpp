#include "mafEventSource.h"
#include "mafObserver.h"
#include "mafEventBase.h"

#include <iostream>

using namespace std;

class mafSubjectTestClass:public mafObject
{
public:
  mafTypeMacro(mafSubjectTestClass,mafObject);
  mafSubjectTestClass():Listeners(this) {}
  mafEventSource *GetListeners() {return Listeners;} 

  void DoSomeThing() {Listeners->InvokeEvent(this,);
  void DoSomeThingElse();
  //void Print(ostream &out) {out<<"Dummy";}
protected:
  mafEventSource Listeners;
};

int main()
{
  
  return 0;
}
