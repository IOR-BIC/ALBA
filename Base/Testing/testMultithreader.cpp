#include "mafMultiThreader.h"
#include "mafMutexLock.h"

#include <iostream>
using namespace std;

class test_data
{
public:
  int thread_id;
  int flag;
  mafMutexLock lock;
};

static void ThreadFunction(mmuThreadInfoStruct *data)
{
  assert(data);
  test_data *mydata=(test_data *)(data->m_UserData);
  mydata->flag=1;
  std::cerr<<"Thread "<<mydata->thread_id<<" (ID="<<data->m_ThreadID<<") waiting for gate\n";
  mydata->lock.Lock(); // wait for gate
  mydata->flag=2;
  for (;mydata->flag;) mafSleep(10); // wait for flag reset
  std::cerr<<"Thread "<<mydata->thread_id<<" (ID="<<data->m_ThreadID<<") is dying\n";
}

int main()
{  
  mafMultiThreader threader;
  test_data data[8];

  for (int i=0;i<8;i++)
  {
    data[i].thread_id=i;
    data[i].flag=0;
    data[i].lock.Lock();
    threader.SpawnThread(ThreadFunction,&(data[i]));
  }
  mafSleep(500); // wait 1sec for all threads to complete their work
  std::cerr<<"Gate1\n";
  for (int i=0;i<8;i++)
  {
    MAF_TEST(data[i].flag==1);
    data[i].lock.Unlock();
  }
  mafSleep(500); // wait 1sec for all threads to complete their work
  
  for (int i=0;i<8;i++)
  {
    MAF_TEST(data[i].flag==2);
  }
  std::cerr<<"Gate2\n";
  for (int i=0;i<8;i++)
  {
   data[i].flag=0;
  }
  mafSleep(500); // wait 1sec for all threads to complete their work

  /*for (int i=0;i<8;i++)
  {
    // close threads
    threader.TerminateThread(data[i].thread_id);
  }*/

  
}
