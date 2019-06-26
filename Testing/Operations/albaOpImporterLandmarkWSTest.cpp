/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmarkWSTest
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

#include "albaOpImporterLandmarkWSTest.h"
#include "albaOpImporterLandmarkWS.h"


#include "albaString.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void albaOpImporterLandmarkWSTest::TestWSImporter() 
//-----------------------------------------------------------
{
	albaOpImporterLandmarkWS *importer=new albaOpImporterLandmarkWS("importer");
	importer->TestModeOn();
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/AL_FileTXT/pbCV1b05al.csv";
	importer->SetFileName(filename.GetCStr());
	importer->Read();
  albaVMELandmarkCloud *node=(albaVMELandmarkCloud *)importer->GetOutput();
  
  CPPUNIT_ASSERT(node->GetNumberOfLandmarks() == 24);
  cppDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterLandmarkWSTest::TestVisibility() 
//-----------------------------------------------------------
{
  albaOpImporterLandmarkWS *importer=new albaOpImporterLandmarkWS("importer");
  importer->TestModeOn();
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/AL_FileTXT/pbCV1b05al.csv";
  importer->SetFileName(filename.GetCStr());
  importer->Read();
  albaVMELandmarkCloud *node=(albaVMELandmarkCloud *)importer->GetOutput();

  bool visible = node->GetLandmarkVisibility(0,7.21);

  CPPUNIT_ASSERT(visible == false);
  cppDEL(importer);
}

//-----------------------------------------------------------
void albaOpImporterLandmarkWSTest::TestCoordinates() 
//-----------------------------------------------------------
{
  albaOpImporterLandmarkWS *importer=new albaOpImporterLandmarkWS("importer");
  importer->TestModeOn();
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/AL_FileTXT/pbCV1b05al.csv";
  importer->SetFileName(filename.GetCStr());
  importer->Read();
  albaVMELandmarkCloud *node=(albaVMELandmarkCloud *)importer->GetOutput();

  double xyz[3], rot[3];
  albaVMELandmark *lm = node->GetLandmark(0);
  lm->GetOutput()->GetPose(xyz,rot,2.94);

  CPPUNIT_ASSERT( fabs(xyz[0] - 36.110333) < 0.01 &&
    fabs(xyz[1] - -1012.215299) < 0.01 &&
    fabs(xyz[2] - 19.816351) < 0.01 );

  //CPPUNIT_ASSERT( albaEquals(xyz[0],144.13) && albaEquals(xyz[1],-1103.8) && albaEquals(xyz[2],1547.2));
  cppDEL(importer);
}


