#include "mafEventBase.h"
#include <iostream>
#include <utilities>

using namespace std;

int main()
{
  
  
  typedef pair<int,const char *> test_type;
  test_type test_data(10,"ciao");
  
  mafObject test_object;

  // test constructor
  mafEventBase event_a(&test_object,5,&test_data);
  MAF_TEST(event_a.GetId()==5);
  MAF_TEST(event_a.GetSender()==&test_object);
  MAF_TEST(event_a.GetData()==&test_data);
  MAF_TEST(event_a.GetSource()==NULL);


  // test copy constructor
  mafEventBase event_b=event_a;
  MAF_TEST(event_b.GetId()==event_a.GetId());
  MAF_TEST(event_b.GetSender()==event_a.GetSender());
  MAF_TEST(event_b.GetData()==event_a.GetData());
  MAF_TEST(event_b.GetSource()==event_a.GetSource());

  // test Set functions
  mafEventBase event_c;
  event_c.SetSender(&test_object);
  event_c.SetId(5);
  event_c.SetData(&test_data);
  MAF_TEST(event_c.GetId()==event_a.GetId());
  MAF_TEST(event_c.GetSender()==event_a.GetSender());
  MAF_TEST(event_c.GetData()==event_a.GetData());
  
  cout<<"Test completed successfully!"<<endl;

  return MAF_OK;
}
