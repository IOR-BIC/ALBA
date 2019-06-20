/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterGRFWSTest
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

#include "albaOpImporterGRFWSTest.h"
#include "albaOpImporterGRFWS.h"


#include "albaString.h"
#include "albaVMEGroup.h"
#include "albaVMEVector.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void albaOpImporterGRFWSTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpImporterGRFWS *importer = new albaOpImporterGRFWS();
  cppDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterGRFWSTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  albaOpImporterGRFWS importer; 
}

//-----------------------------------------------------------
void albaOpImporterGRFWSTest::TestRead() 
//-----------------------------------------------------------
{
	albaOpImporterGRFWS *importer=new albaOpImporterGRFWS("importer");
	importer->TestModeOn();
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_ImporterGRF/TestPbCV1b06grf.txt";
	importer->SetFileName(filename.GetCStr());
	importer->Read();

  albaVMEGroup *group=(albaVMEGroup *)importer->GetOutput();

  CPPUNIT_ASSERT( group->GetNumberOfChildren() == 2);

  albaVMEVector *vector1 = (albaVMEVector*)group->GetChild(0)->GetFirstChild();
  vector1->SetTimeStamp(4.65);
  double module1 = vector1->GetModule();
  CPPUNIT_ASSERT( fabs(module1 - 435.439) < 0.01);

  vector1->SetTimeStamp(4.55);
  module1 = vector1->GetModule();
  CPPUNIT_ASSERT( fabs(module1 - 821.6602) < 0.01);

  albaVMEVector *vector2 = (albaVMEVector*)group->GetChild(1)->GetFirstChild();
  vector2->SetTimeStamp(4.57);
  double module2 = vector2->GetModule();
  CPPUNIT_ASSERT( fabs(module2 - 35.894) < 0.01);

  vector2->SetTimeStamp(4.61);
  module2 = vector2->GetModule();
  CPPUNIT_ASSERT( fabs(module2 - 495.637) < 0.01);

  cppDEL(importer);
}