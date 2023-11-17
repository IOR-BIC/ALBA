/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewPlotTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"
#include <cppunit/config/SourcePrefix.h>

#include "TimeSetTest.h"
#include "mmuTimeSet.h"
#include <iostream>

void TimeSetTest::TimeSetMainTest()
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};

  // mmuTimeSet is a container for albaTimeStamps
  mmuTimeSet tset;
  int i;
  for (i=0;i<6;i++)
    tset.Insert(tarray[i]);

  CPPUNIT_ASSERT(tset.GetNumberOfTimeStamps()==6);

  // test ordering
  albaTimeStamp old_t;
  mmuTimeSet::Iterator it;
  for (old_t=0,it=tset.Begin();it!=tset.End();it++)
  {
    CPPUNIT_ASSERT(old_t<*it);
    old_t=*it;
  }
  
  // test find by index
  CPPUNIT_ASSERT(tset.FindTimeStampIndex(1.2)==2);
  CPPUNIT_ASSERT(tset.FindTimeStampIndex(.5)==0);
  CPPUNIT_ASSERT(tset.FindTimeStampIndex(3.6)==5);

  // test insertion at begin
  tset.Prepend(.3); // inserted in constant time
  CPPUNIT_ASSERT(tset.FindTimeStampIndex(.3)==0); 
  tset.Prepend(2); // inserted in 2*log(N) time
  CPPUNIT_ASSERT(tset.FindTimeStampIndex(2)==5); 

  // test insertion at end
  tset.Append(4); // inserted in constant time
  CPPUNIT_ASSERT(tset.FindTimeStampIndex(4)==8); 
  tset.Append(.2); // inserted in 2*log(N) time
  CPPUNIT_ASSERT(tset.FindTimeStampIndex(.2)==0); 

  // check we have the correct number of time stamps
  CPPUNIT_ASSERT(tset.GetNumberOfTimeStamps()==10);

  albaTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};


  // check for set sort integrity
  for (i=0,it=tset.Begin();it!=tset.End();it++,i++)
  {
    CPPUNIT_ASSERT(final_tarray[i]==*it);
  }

  // test the find algorithms
  CPPUNIT_ASSERT(tset.GetTimeStampBefore(2.4)==2.0);
  CPPUNIT_ASSERT(tset.GetNearestTimeStamp(2.4)==2.5);

  // test search of an element out of bounds
  CPPUNIT_ASSERT(tset.GetTimeStampBefore(.1)==.2); 
  CPPUNIT_ASSERT(tset.GetNearestTimeStamp(.1)==.2);

  CPPUNIT_ASSERT(tset.GetTimeStampBefore(5)==4); 
  CPPUNIT_ASSERT(tset.GetNearestTimeStamp(5)==4);

  // test merge
  mmuTimeSet tset2;
  tset2.Append(1.5); 
  tset2.Append(1.8);
  tset2.Append(2.3);
  tset2.Append(2.5);

  mmuTimeSet tset_result;
  mmuTimeSet::Merge(tset,tset2,tset_result);

  CPPUNIT_ASSERT(tset_result.GetNumberOfTimeStamps()==12);

  for (old_t=0,it=tset_result.Begin();it!=tset_result.End();it++)
  {
    CPPUNIT_ASSERT(old_t<*it);
    old_t=*it;
    // test the time to be in one of the two original sets
    CPPUNIT_ASSERT((tset.FindTimeStamp(*it)!=tset.End())||(tset2.FindTimeStamp(*it)!=tset2.End()));
  }

  for (it=tset.Begin();it!=tset.End();it++)
  {
    CPPUNIT_ASSERT(tset_result.FindTimeStamp(*it)!=tset_result.End());
  }

  for (it=tset2.Begin();it!=tset2.End();it++)
  {
    CPPUNIT_ASSERT(tset_result.FindTimeStamp(*it)!=tset_result.End());
  }
  
  // test copy
  mmuTimeSet tset_copia=tset;
  CPPUNIT_ASSERT(tset==tset_copia);

  // test second Merge syntax
  tset_copia.Merge(tset2);
  CPPUNIT_ASSERT(tset_result==tset_copia);

  // test third Merge syntax
  mmuTimeSet::Merge(tset,tset2,tset2);
  CPPUNIT_ASSERT(tset_result==tset2);

  // test removal
  it=tset_result.FindTimeStamp(2.0);
  CPPUNIT_ASSERT(it!=tset_result.End());
  tset_result.Remove(it);
  CPPUNIT_ASSERT(tset_result.GetNumberOfTimeStamps()==11);
  CPPUNIT_ASSERT(tset_result.FindTimeStamp(2.0)==tset_result.End());
}

