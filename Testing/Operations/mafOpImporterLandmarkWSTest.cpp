/*=========================================================================

 Program: MAF2Medical
 Module: mafOpImporterLandmarkWSTest
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

#include "mafOpImporterLandmarkWSTest.h"
#include "mafOpImporterLandmarkWS.h"


#include "mafString.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void mafOpImporterLandmarkWSTest::TestWSImporter() 
//-----------------------------------------------------------
{
	mafOpImporterLandmarkWS *importer=new mafOpImporterLandmarkWS("importer");
	importer->TestModeOn();
	mafString filename=MAF_DATA_ROOT;
  filename<<"/AL_FileTXT/pbCV1b05al.csv";
	importer->SetFileName(filename.GetCStr());
	importer->Read();
  mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();
  
  CPPUNIT_ASSERT(node->GetNumberOfLandmarks() == 24);
  cppDEL(importer);
}
//-----------------------------------------------------------
void mafOpImporterLandmarkWSTest::TestVisibility() 
//-----------------------------------------------------------
{
  mafOpImporterLandmarkWS *importer=new mafOpImporterLandmarkWS("importer");
  importer->TestModeOn();
  mafString filename=MAF_DATA_ROOT;
  filename<<"/AL_FileTXT/pbCV1b05al.csv";
  importer->SetFileName(filename.GetCStr());
  importer->Read();
  mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();

  bool visible = node->GetLandmarkVisibility(0,7.21);

  CPPUNIT_ASSERT(visible == false);
  cppDEL(importer);
}

//-----------------------------------------------------------
void mafOpImporterLandmarkWSTest::TestCoordinates() 
//-----------------------------------------------------------
{
  mafOpImporterLandmarkWS *importer=new mafOpImporterLandmarkWS("importer");
  importer->TestModeOn();
  mafString filename=MAF_DATA_ROOT;
  filename<<"/AL_FileTXT/pbCV1b05al.csv";
  importer->SetFileName(filename.GetCStr());
  importer->Read();
  mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();

  double xyz[3], rot[3];
  mafVMELandmark *lm = node->GetLandmark(0);
  lm->GetOutput()->GetPose(xyz,rot,2.94);

  CPPUNIT_ASSERT( fabs(xyz[0] - 36.110333) < 0.01 &&
    fabs(xyz[1] - -1012.215299) < 0.01 &&
    fabs(xyz[2] - 19.816351) < 0.01 );

  //CPPUNIT_ASSERT( mafEquals(xyz[0],144.13) && mafEquals(xyz[1],-1103.8) && mafEquals(xyz[2],1547.2));
  cppDEL(importer);
}


