/*=========================================================================

 Program: MAF2
 Module: mafOpImporterAnalogWSTest
 Authors: Roberto Mucci
 
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

#include "mafOpImporterAnalogWSTest.h"
#include "mafOpImporterAnalogWS.h"
#include <vnl/vnl_vector.h>

#include "mafString.h"
#include "mafVMEAnalog.h"
#include "mafVMEOutputScalarMatrix.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void mafOpImporterAnalogWSTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpImporterAnalogWS *importer = new mafOpImporterAnalogWS("importer");
  cppDEL(importer);
}
//-----------------------------------------------------------
void mafOpImporterAnalogWSTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  mafOpImporterAnalogWS importer; 
}

//-----------------------------------------------------------
void mafOpImporterAnalogWSTest::TestRead() 
//-----------------------------------------------------------
{
	mafOpImporterAnalogWS *importer=new mafOpImporterAnalogWS("importer");
	importer->TestModeOn();
	mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_ImporterAnalogWS/pbCV1b06emg_ridotto.csv";
	importer->SetFileName(filename.GetCStr());
	importer->Read();

  mafVMEAnalog *analogScalar = (mafVMEAnalog *)importer->GetOutput();
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