/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterGRFWSTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpImporterGRFWSTest.h"
#include "medOpImporterGRFWS.h"


#include "mafString.h"
#include "mafVMEGroup.h"
#include "mafVMEVector.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void medOpImporterGRFWSTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpImporterGRFWS *importer = new medOpImporterGRFWS();
  cppDEL(importer);
}
//-----------------------------------------------------------
void medOpImporterGRFWSTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  medOpImporterGRFWS importer; 
}

//-----------------------------------------------------------
void medOpImporterGRFWSTest::TestRead() 
//-----------------------------------------------------------
{
	medOpImporterGRFWS *importer=new medOpImporterGRFWS("importer");
	importer->TestModeOn();
	mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_ImporterGRF/TestPbCV1b06grf.txt";
	importer->SetFileName(filename.GetCStr());
	importer->Read();

  mafVMEGroup *group=(mafVMEGroup *)importer->GetOutput();

  CPPUNIT_ASSERT( group->GetNumberOfChildren() == 2);

  mafVMEVector *vector1 = (mafVMEVector*)group->GetChild(0)->GetFirstChild();
  vector1->SetTimeStamp(4.65);
  double module1 = vector1->GetModule();
  CPPUNIT_ASSERT( fabs(module1 - 435.439) < 0.01);

  vector1->SetTimeStamp(4.55);
  module1 = vector1->GetModule();
  CPPUNIT_ASSERT( fabs(module1 - 821.6602) < 0.01);

  mafVMEVector *vector2 = (mafVMEVector*)group->GetChild(1)->GetFirstChild();
  vector2->SetTimeStamp(4.57);
  double module2 = vector2->GetModule();
  CPPUNIT_ASSERT( fabs(module2 - 35.894) < 0.01);

  vector2->SetTimeStamp(4.61);
  module2 = vector2->GetModule();
  CPPUNIT_ASSERT( fabs(module2 - 495.637) < 0.01);

  cppDEL(importer);
}