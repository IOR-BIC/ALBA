/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMotionDataTest
 Authors: Simone Brazzale
 
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

#include "mafopImporterMotionDataTest.h"
#include "mafOpImporterMotionData.h"
#include "mafVMERawMotionData.h"
#include "mafVMELandmarkCloud.h"
#include "mafDataVector.h"
#include "mafVMELandmark.h"

#include <assert.h>

//-----------------------------------------------------------
void mafOpImporterMotionDataTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpImporterMotionData<mafVMERawMotionData> *importer = new mafOpImporterMotionData<mafVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt");
  cppDEL(importer);
}
//-----------------------------------------------------------
void mafOpImporterMotionDataTest::TestCopy() 
//-----------------------------------------------------------
{
  mafOpImporterMotionData<mafVMERawMotionData> *importer = new mafOpImporterMotionData<mafVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt");
  mafOpImporterMotionData<mafVMERawMotionData> *importer_copy = NULL;

  importer->TestModeOn();
  importer_copy = ((mafOpImporterMotionData<mafVMERawMotionData>*)importer->Copy());

  CPPUNIT_ASSERT( importer_copy->GetType() == importer->GetType() );

  cppDEL(importer_copy);
  cppDEL(importer);
}
//-----------------------------------------------------------
void mafOpImporterMotionDataTest::TestOnRawMotionData() 
//-----------------------------------------------------------
{
  mafOpImporterMotionData<mafVMERawMotionData> *importer = new mafOpImporterMotionData<mafVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt");
  importer->TestModeOn();

  mafString filename=MAF_DATA_ROOT;
  filename<<"/RAW_MAL/Fprg3bsi_NOT_USED.man";
  mafString fileDict=MAF_DATA_ROOT;
  fileDict<<"/RAW_MAL/DICT_FPRG3BSL_421_NOT_USED.TXT";
  importer->SetFileName(filename.GetCStr());
  importer->SetDictionaryName(fileDict.GetCStr());
  importer->SetDictionaryFlagOn();

  importer->OpDo();
  CPPUNIT_ASSERT(importer->GetOutput()!=NULL);

  mafVMERawMotionData* vme_raw;
  vme_raw = (mafVMERawMotionData*)importer->GetOutput();

  //child control
  long n = vme_raw->GetNumberOfChildren();
  CPPUNIT_ASSERT(n == 5); // 5 is the number of children taken from the typology in the dictionary

  int resultNameChild = -5;

  for(int i = 0; i < vme_raw->GetNumberOfChildren(); i++)
  {
    mafString name = vme_raw->GetChild(i)->GetName();
    if(name == "IPE" || name == "LSH" || name == "LTH" || name == "RSH" || name == "RTH")
      resultNameChild++;
  }
  CPPUNIT_ASSERT(resultNameChild == 0);

  int totalPoints = 22; //taken from man file
  int calculatedSum = 0;
  for(int i=0; i< 5; i++)
    calculatedSum += ((mafVMELandmarkCloud *)vme_raw->GetChild(i))->GetNumberOfLandmarks();

  CPPUNIT_ASSERT(calculatedSum == totalPoints);

  // test a coordinate in at first time step of a landmark to accept the validity of the reader
  // In this case I can say that this reader works correctly like a time-variant vme

  // IASL	IPE at time step 1
  double x,y,z;
  x = -390.13;
  y = 761.53;
  z = -289.80;
           
  if (importer->GetTestMode() == true)
  {
     ((mafVMELandmarkCloud *)vme_raw->GetChild(0))->TestModeOn();
  }

  mafVMELandmark *landmark = ((mafVMELandmarkCloud *)vme_raw->GetChild(0))->GetLandmark(0);
  
  mafString nameLand = landmark->GetName();
  CPPUNIT_ASSERT(nameLand == "IASR");

  double xyz[3];
  double rot[3];
  landmark->GetOutput()->GetPose(xyz , rot , 1);

  CPPUNIT_ASSERT(xyz[0] <= -390.12 && xyz[0] >= -390.14 &&
                 xyz[1] >= 761.52 &&  xyz[1] <= 761.54 &&
                 xyz[2] <= -289.79 && xyz[2] >= -289.81);
  
  cppDEL(importer);
}
