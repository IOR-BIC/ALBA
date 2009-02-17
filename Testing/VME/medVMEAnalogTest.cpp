/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEAnalogTest.cpp,v $
Language:  C++
Date:      $Date: 2009-02-17 16:52:55 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medVMEAnalogTest.h"
#include <iostream>

#include "medVMEAnalog.h"
#include <vnl\vnl_matrix.h>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void medVMEAnalogTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void medVMEAnalogTest::tearDown()
//----------------------------------------------------------------------------
{
}

//---------------------------------------------------------
void medVMEAnalogTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  medVMEAnalog *analog = NULL;
  mafNEW(analog);
  mafDEL(analog);
}
//---------------------------------------------------------
void medVMEAnalogTest::TestPrint()
//---------------------------------------------------------
{
  medVMEAnalog *analog = NULL;
  mafNEW(analog);
  //only print
  analog->Print(std::cout);
  mafDEL(analog);
}
//---------------------------------------------------------
void medVMEAnalogTest::TestGetLocalTimeBounds()
//---------------------------------------------------------
{
  medVMEAnalog *analog = NULL;
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
void medVMEAnalogTest::TestIsAnimated()
//---------------------------------------------------------
{
  medVMEAnalog *analog = NULL;
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
void medVMEAnalogTest::TestGetTimeBounds()
//---------------------------------------------------------
{
  //actually this method invokes TestGetLocalTimeBounds, so it determines the same results
  medVMEAnalog *analog = NULL;
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
void medVMEAnalogTest::TestGetLocalTimeStamps()
//---------------------------------------------------------
{
  //actually this method invokes TestGetLocalTimeBounds, so it determines the same results
  medVMEAnalog *analog = NULL;
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
