/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterAnalogWSTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpImporterAnalogWSTest.h"
#include "albaOpImporterAnalogWS.h"
#include <vnl/vnl_vector.h>

#include "albaString.h"
#include "albaVMEAnalog.h"
#include "albaVMEOutputScalarMatrix.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void albaOpImporterAnalogWSTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpImporterAnalogWS *importer = new albaOpImporterAnalogWS("importer");
  cppDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterAnalogWSTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  albaOpImporterAnalogWS importer; 
}

//-----------------------------------------------------------
void albaOpImporterAnalogWSTest::TestRead() 
//-----------------------------------------------------------
{
	albaOpImporterAnalogWS *importer=new albaOpImporterAnalogWS("importer");
	importer->TestModeOn();
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_ImporterAnalogWS/pbCV1b06emg_ridotto.csv";
	importer->SetFileName(filename.GetCStr());
	importer->Read();

  albaVMEAnalog *analogScalar = (albaVMEAnalog *)importer->GetOutput();
  analogScalar->Update();

  //check time vector
	double timeVector[2]; 
	timeVector[0] = analogScalar->GetScalarOutput()->GetScalarData().get(0, 0);
	timeVector[1] = analogScalar->GetScalarOutput()->GetScalarData().get(0, 37);
  CPPUNIT_ASSERT( fabs(timeVector[0] - 0.0005) < 0.01 && fabs(timeVector[1] - 0.098) < 0.01);

  //check scalar values
	double scalarVector[2];
	scalarVector[0] = analogScalar->GetScalarOutput()->GetScalarData().get(1, 0);
	scalarVector[1] = analogScalar->GetScalarOutput()->GetScalarData().get(1, 37);
	CPPUNIT_ASSERT(fabs(scalarVector[0] - 0.07795) < 0.01 && fabs(scalarVector[1] - 0.175388) < 0.01);

  //check last scalar values
  int num = analogScalar->GetScalarOutput()->GetScalarData().rows();
	double scalarVectorEnd[2];
	scalarVectorEnd[0] = analogScalar->GetScalarOutput()->GetScalarData().get(num-1, 0);
	scalarVectorEnd[1] = analogScalar->GetScalarOutput()->GetScalarData().get(num-1, 37);
	CPPUNIT_ASSERT(fabs(scalarVectorEnd[0] - 237) < 0.01 && fabs(scalarVectorEnd[1] - 247) < 0.01);
  
	cppDEL(importer);
}