#include "mafEventSource.h"
#include "mafObserver.h"
#include "mafEventBase.h"

#include <iostream>

using namespace std;

class mafSubjectTestClass:public mafObject
{
public:
  mafTypeMacro(mafSubjectTestClass,mafObject);
  mafSubjectTestClass():EventDummy(this),EventFoo(this) {}
  mafEventSource &GetEventDummy() {return EventDummy;} 
  mafEventSource &GetEventFoo() {return EventFoo;} 

  enum my_ids {ID_MY_OWN=ID_NO_EVENT+1, ID_DUMMY, ID_FOO};

  void DoSomeThing() {EventDummy.InvokeEvent(this);}
  void DoSomeThingElse();
  //void Print(ostream &out) {out<<"Dummy";}
protected:
  mafEventSource EventDummy;
  mafEventSource EventFoo;
};

mafCxxTypeMacro(mafSubjectTestClass);

int main()
{
  mafSubjectTestClass first_subject;
  mafSubjectTestClass second_subject;

  return 0;
}
