/*=========================================================================

 Program: MAF2
 Module: mafVMEAnalogTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVMEAnalogTest.h"
#include <iostream>

#include "mafVMEAnalog.h"
#include <vnl\vnl_matrix.h>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafVMEAnalogTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void mafVMEAnalogTest::tearDown()
//----------------------------------------------------------------------------
{
}

//---------------------------------------------------------
void mafVMEAnalogTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafVMEAnalog *analog = NULL;
  mafNEW(analog);
  mafDEL(analog);
}
//---------------------------------------------------------
void mafVMEAnalogTest::TestPrint()
//---------------------------------------------------------
{
  mafVMEAnalog *analog = NULL;
  mafNEW(analog);
  //only print
  analog->Print(std::cout);
  mafDEL(analog);
}
//---------------------------------------------------------
void mafVMEAnalogTest::TestGetLocalTimeBounds()
//---------------------------------------------------------
{
  mafVMEAnalog *analog = NULL;
  vnl_matrix<double> emgMatrix;
  int rows = 5;
  int nTimeStamps = 3;
  emgMatrix.set_size(rows-1 , nTimeStamps);

  int i=0;
  int j=0;
  int count = 0;
  for(;i<=rows;i++)
  {
    for(;j<nTimeStamps;j++)
    {
      emgMatrix.put(i,j,count++);  //timeStamp is the first line-> 0 1 2
    }
  }

  mafNEW(analog);
  analog->SetData(emgMatrix,0);
  analog->Update();

  double tbounds[2];
  analog->GetLocalTimeBounds(tbounds);

  m_Result = tbounds[0] == 0 && tbounds[1] == 2; //control time bounds 0 - 2 
  TEST_RESULT;

  mafDEL(analog);
}
//---------------------------------------------------------
void mafVMEAnalogTest::TestIsAnimated()
//---------------------------------------------------------
{
  mafVMEAnalog *analog = NULL;
  vnl_matrix<double> emgMatrix;
  int rows = 5;
  int nTimeStamps = 3;
  emgMatrix.set_size(rows-1 , nTimeStamps);

  int i=0;
  int j=0;
  int count = 0;
  for(;i<=rows;i++)
  {
    for(;j<nTimeStamps;j++)
    {
      emgMatrix.put(i,j,count++);  //timeStamp is the first line-> 0 1 2
    }
  }

  mafNEW(analog);
  analog->SetData(emgMatrix,0);
  analog->Update();

  double tbounds[2];
  analog->GetLocalTimeBounds(tbounds);

  m_Result = analog->IsAnimated(); // analog is animated
  TEST_RESULT;

  mafDEL(analog);
}
//---------------------------------------------------------
void mafVMEAnalogTest::TestGetTimeBounds()
//---------------------------------------------------------
{
  //actually this method invokes TestGetLocalTimeBounds, so it determines the same results
  mafVMEAnalog *analog = NULL;
  vnl_matrix<double> emgMatrix;
  int rows = 5;
  int nTimeStamps = 3;
  emgMatrix.set_size(rows-1 , nTimeStamps);

  int i=0;
  int j=0;
  int count = 0;
  for(;i<=rows;i++)
  {
    for(;j<nTimeStamps;j++)
    {
      emgMatrix.put(i,j,count++);  //timeStamp is the first line-> 0 1 2
    }
  }

  mafNEW(analog);
  analog->SetData(emgMatrix,0);
  analog->Update();

  double tbounds[2];
  analog->GetTimeBounds(tbounds);

  m_Result = tbounds[0] == 0 && tbounds[1] == 2; //control time bounds 0 - 2 
  TEST_RESULT;

  mafDEL(analog);
}
//---------------------------------------------------------
void mafVMEAnalogTest::TestGetLocalTimeStamps()
//---------------------------------------------------------
{
  //actually this method invokes TestGetLocalTimeBounds, so it determines the same results
  mafVMEAnalog *analog = NULL;
  vnl_matrix<double> emgMatrix;
  int rows = 5;
  int nTimeStamps = 3;
  emgMatrix.set_size(rows-1 , nTimeStamps);

  int i=0;
  int j=0;
  int count = 0;
  for(;i<=rows;i++)
  {
    for(;j<nTimeStamps;j++)
    {
      emgMatrix.put(i,j,count++);  //timeStamp is the first line-> 0 1 2
    }
  }

  mafNEW(analog);
  analog->SetData(emgMatrix,0);
  analog->Update();

  std::vector<double> timeVector;
  analog->GetLocalTimeStamps(timeVector);
  
    //control time vector
  m_Result = timeVector.size() == 3 &&\
             timeVector[0] == 0 &&\
             timeVector[1] == 1 &&\
             timeVector[2] == 2;

  TEST_RESULT;

  mafDEL(analog);
}
