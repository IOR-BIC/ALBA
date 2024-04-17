/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMotionDataTest
 Authors: Simone Brazzale
 
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

#include "albaOpImporterMotionDataTest.h"
#include "albaOpImporterMotionData.h"
#include "albaVMERawMotionData.h"
#include "albaVMELandmarkCloud.h"
#include "albaDataVector.h"
#include "albaVMELandmark.h"

#include <assert.h>

//-----------------------------------------------------------
void albaOpImporterMotionDataTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpImporterMotionData<albaVMERawMotionData> *importer = new albaOpImporterMotionData<albaVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt");
  cppDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterMotionDataTest::TestCopy() 
//-----------------------------------------------------------
{
  albaOpImporterMotionData<albaVMERawMotionData> *importer = new albaOpImporterMotionData<albaVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt");
  albaOpImporterMotionData<albaVMERawMotionData> *importer_copy = NULL;

  importer->TestModeOn();
  importer_copy = ((albaOpImporterMotionData<albaVMERawMotionData>*)importer->Copy());

  CPPUNIT_ASSERT( importer_copy->GetType() == importer->GetType() );

  cppDEL(importer_copy);
  cppDEL(importer);
}
//-----------------------------------------------------------
void albaOpImporterMotionDataTest::TestOnRawMotionData() 
//-----------------------------------------------------------
{
  albaOpImporterMotionData<albaVMERawMotionData> *importer = new albaOpImporterMotionData<albaVMERawMotionData>("Raw Motion Data", "RAW Motion Data (*.MAN)|*.MAN", "Dictionary (*.txt)|*.txt");
  importer->TestModeOn();

  albaString filename=ALBA_DATA_ROOT;
  filename<<"/RAW_MAL/Fprg3bsi_NOT_USED.man";
  albaString fileDict=ALBA_DATA_ROOT;
  fileDict<<"/RAW_MAL/DICT_FPRG3BSL_421_NOT_USED.TXT";
  importer->SetFileName(filename.GetCStr());
  importer->SetDictionaryName(fileDict.GetCStr());
  importer->SetDictionaryFlagOn();

  importer->OpDo();
  CPPUNIT_ASSERT(importer->GetOutput()!=NULL);

  albaVMERawMotionData* vme_raw;
  vme_raw = (albaVMERawMotionData*)importer->GetOutput();

  //child control
  long n = vme_raw->GetNumberOfChildren();
  CPPUNIT_ASSERT(n == 5); // 5 is the number of children taken from the typology in the dictionary

  int resultNameChild = -5;

  for(int i = 0; i < vme_raw->GetNumberOfChildren(); i++)
  {
    albaString name = vme_raw->GetChild(i)->GetName();
    if(name == "IPE" || name == "LSH" || name == "LTH" || name == "RSH" || name == "RTH")
      resultNameChild++;
  }
  CPPUNIT_ASSERT(resultNameChild == 0);

  int totalPoints = 22; //taken from man file
  int calculatedSum = 0;
  for(int i=0; i< 5; i++)
    calculatedSum += ((albaVMELandmarkCloud *)vme_raw->GetChild(i))->GetNumberOfLandmarks();

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
     ((albaVMELandmarkCloud *)vme_raw->GetChild(0))->TestModeOn();
  }

  albaVMELandmark *landmark = ((albaVMELandmarkCloud *)vme_raw->GetChild(0))->GetLandmark(0);
  
  albaString nameLand = landmark->GetName();
  CPPUNIT_ASSERT(nameLand == "IASR");

  double xyz[3];
  double rot[3];
  landmark->GetOutput()->GetPose(xyz , rot , 1);

  CPPUNIT_ASSERT(xyz[0] <= -390.12 && xyz[0] >= -390.14 &&
                 xyz[1] >= 761.52 &&  xyz[1] <= 761.54 &&
                 xyz[2] <= -289.79 && xyz[2] >= -289.81);
  
  cppDEL(importer);
}
