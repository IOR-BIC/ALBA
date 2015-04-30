/*=========================================================================

 Program: MAF2
 Module: mafOpImporterLandmarkTest
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

#include "mafOpImporterLandmarkTest.h"
#include "mafOpImporterLandmark.h"


#include "mafString.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpImporterLandmarkTest::Test() 
//-----------------------------------------------------------
{
	mafOpImporterLandmark *importer=new mafOpImporterLandmark("importer");
	importer->TestModeOn();

	mafString filename=MAF_DATA_ROOT;
  filename<<"/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->Read();
	mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(node->GetNumberOfLandmarks() == 38);

  node->Open();
  double xyz[3], rot[3];
  node->GetLandmark(19)->GetOutput()->GetPose(xyz,rot,0);

  
  double x = -21.5165;	
  double y = 106.0285;
  double z = 320.5479;

  CPPUNIT_ASSERT( fabs(xyz[0] - x) < 0.01 &&
                  fabs(xyz[1] - y) < 0.01 &&
                  fabs(xyz[2] - z) < 0.01 );

  cppDEL(importer);
}
//-----------------------------------------------------------
void mafOpImporterLandmarkTest::TestTimeVariant() 
//-----------------------------------------------------------
{
	mafOpImporterLandmark *importer=new mafOpImporterLandmark("importer");
	importer->TestModeOn();
	mafString filename=MAF_DATA_ROOT;
  filename<<"/RAW_MAL/Export.txt";
	importer->SetFileName(filename.GetCStr());
	importer->Read();
	mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(node->GetNumberOfLandmarks() == 22);

  node->Open();
  double xyz[3], rot[3];
  node->GetLandmark(0)->GetOutput()->GetPose(xyz,rot,10);
  mafString name = node->GetLandmark(0)->GetName();
  
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
void mafOpImporterLandmarkTest::TestUnTag() 
//-----------------------------------------------------------
{
	mafOpImporterLandmark *importer=new mafOpImporterLandmark("importer");
	importer->TestModeOn();
	mafString filename=MAF_DATA_ROOT;
  filename<<"/RAW_MAL/cloud_NOT_TAGGED";
	importer->SetFileName(filename.GetCStr());
	importer->ReadWithoutTag();
	mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(node->GetNumberOfLandmarks() == 30);

  node->Open();
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
