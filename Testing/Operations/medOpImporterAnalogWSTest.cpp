/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterAnalogWSTest.cpp,v $
Language:  C++
Date:      $Date: 2010-09-08 14:57:46 $
Version:   $Revision: 1.1.2.2 $
Authors:   Roberto Mucci
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

#include "medOpImporterAnalogWSTest.h"
#include "medOpImporterAnalogWS.h"
#include <vnl/vnl_vector.h>

#include "mafString.h"
#include "medVMEAnalog.h"
#include "mafVMEOutputScalarMatrix.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void medOpImporterAnalogWSTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpImporterAnalogWS *importer = new medOpImporterAnalogWS("importer");
  cppDEL(importer);
}
//-----------------------------------------------------------
void medOpImporterAnalogWSTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  medOpImporterAnalogWS importer; 
}

//-----------------------------------------------------------
void medOpImporterAnalogWSTest::TestRead() 
//-----------------------------------------------------------
{
	medOpImporterAnalogWS *importer=new medOpImporterAnalogWS("importer");
	importer->TestModeOn();
	mafString filename=MED_DATA_ROOT;
  filename<<"/Test_ImporterAnalogWS/pbCV1b06emg_ridotto.csv";
	importer->SetFileName(filename.GetCStr());
	importer->Read();

  medVMEAnalog *analogScalar = (medVMEAnalog *)importer->GetOutput();
  analogScalar->Update();

  //check time vector
  vnl_vector<double> timeVector = analogScalar->GetScalarOutput()->GetScalarData().get_row(0);
  CPPUNIT_ASSERT( fabs(timeVector[0] - 0.0005) < 0.01 && fabs(timeVector[timeVector.size()-1] - 0.098) < 0.01);

  //check scalar values
  vnl_vector<double> scalarVector = analogScalar->GetScalarOutput()->GetScalarData().get_row(1);
  CPPUNIT_ASSERT( fabs(scalarVector[0] - 0.07795) < 0.01 && fabs(scalarVector[scalarVector.size()-1] - 0.175388) < 0.01);

  //check last scalar values
  int num = analogScalar->GetScalarOutput()->GetScalarData().rows();
  vnl_vector<double> scalarVectorEnd = analogScalar->GetScalarOutput()->GetScalarData().get_row(num-1);

  double n = scalarVectorEnd[0];
  double n1 = scalarVectorEnd[scalarVectorEnd.size()-1];

  CPPUNIT_ASSERT( fabs(scalarVectorEnd[0] - 237) < 0.01 && fabs(scalarVectorEnd[scalarVectorEnd.size()-1] - 247) < 0.01);
  cppDEL(importer);
}