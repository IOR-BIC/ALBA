/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmarkTest
 Authors: Daniele Giunchi
 
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

#include "albaOpImporterLandmarkTest.h"
#include "albaOpImporterLandmark.h"


#include "albaString.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpImporterLandmarkTest::Test() 
//-----------------------------------------------------------
{
	albaOpImporterLandmark *importer=new albaOpImporterLandmark("importer");
	importer->TestModeOn();

	albaString filename=ALBA_DATA_ROOT;
  filename<<"/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->SetTypeSeparation(3);
	importer->Read();
	albaVMELandmarkCloud *node=(albaVMELandmarkCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(node->GetNumberOfLandmarks() == 38);

  double xyz[3];
	node->GetLandmarkPosition(19, xyz);

  
  double x = -21.5165;	
  double y = 106.0285;
  double z = 320.5479;

  CPPUNIT_ASSERT( fabs(xyz[0] - x) < 0.01 &&
                  fabs(xyz[1] - y) < 0.01 &&
                  fabs(xyz[2] - z) < 0.01 );

  cppDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterLandmarkTest::TestTimeVariant() 
//-----------------------------------------------------------
{
	albaOpImporterLandmark *importer=new albaOpImporterLandmark("importer");
	importer->TestModeOn();
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/RAW_MAL/Export.txt";
	importer->SetFileName(filename.GetCStr());
	importer->SetTypeSeparation(3);
	importer->Read();
	albaVMELandmarkCloud *node=(albaVMELandmarkCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(node->GetNumberOfLandmarks() == 22);

  double xyz[3], rot[3];
  node->GetLandmark(0)->GetOutput()->GetPose(xyz,rot,10);
  albaString name = node->GetLandmark(0)->GetName();
  
  CPPUNIT_ASSERT(name == "IASR");
  
  double x = -375.79;	
  double y = 754.39;
  double z = -268.47;

  CPPUNIT_ASSERT( fabs(xyz[0] - x) < 0.01 &&
                  fabs(xyz[1] - y) < 0.01 &&
                  fabs(xyz[2] - z) < 0.01 );


  cppDEL(importer);

}
//-----------------------------------------------------------
void albaOpImporterLandmarkTest::TestUnTag() 
//-----------------------------------------------------------
{
	albaOpImporterLandmark *importer=new albaOpImporterLandmark("importer");
	importer->TestModeOn();
	albaString filename=ALBA_DATA_ROOT;
  filename<<"/RAW_MAL/cloud_NOT_TAGGED";
	importer->SetFileName(filename.GetCStr());
	importer->SetTypeSeparation(1);
	importer->SetOnlyCoordinates(true);
	importer->Read();
	albaVMELandmarkCloud *node=(albaVMELandmarkCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(node->GetNumberOfLandmarks() == 30);

  double xyz[3], rot[3];
  node->GetLandmark(0)->GetOutput()->GetPose(xyz,rot,0);

  
  double x = 404.436;	
  double y = 287.957;
  double z = 94.093;

  CPPUNIT_ASSERT( fabs(xyz[0] - x) < 0.01 &&
                  fabs(xyz[1] - y) < 0.01 &&
                  fabs(xyz[2] - z) < 0.01 );

  cppDEL(importer);

}
