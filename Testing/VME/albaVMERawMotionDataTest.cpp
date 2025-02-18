/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMERawMotionDataTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaVMERawMotionDataTest.h"
#include <iostream>

#include "albaVMERawMotionData.h"
#include "albaVMELandmarkCloud.h"
#include "albaDataVector.h"
#include "albaVMELandmark.h"
 

void albaVMERawMotionDataTest::Test()
{
  albaVMERawMotionData *vmeRawMotionData;

  // create reader
  albaNEW(vmeRawMotionData);

  // try to set this data to the volume
  int returnValue = 1;

  albaString filename=ALBA_DATA_ROOT;
  filename<<"/RAW_MAL/Fprg3bsi.man";

  albaString fileDict=ALBA_DATA_ROOT;
  fileDict<<"/RAW_MAL/DICT_FPRG3BSL_421.TXT";

  vmeRawMotionData->DictionaryOn();

  vmeRawMotionData->SetFileName(filename);
	vmeRawMotionData->SetDictionaryFileName(fileDict);

  // read control
  returnValue = vmeRawMotionData->Read();
  CPPUNIT_ASSERT(returnValue == ALBA_OK);  // 0 if read is ok

  //child control
  long n = vmeRawMotionData->GetNumberOfChildren();
  CPPUNIT_ASSERT(n == 5); // 5 is the number of children taken from the typology in the dictionary

  int resultNameChild = -5;

  for(int i = 0; i < vmeRawMotionData->GetNumberOfChildren(); i++)
  {
    albaString name = vmeRawMotionData->GetChild(i)->GetName();
    if(name == "IPE" ||
        name == "LSH" ||
         name == "LTH" ||
          name == "RSH" ||
           name == "RTH")
      resultNameChild++;
  }
  CPPUNIT_ASSERT(resultNameChild == 0);

  int totalPoints = 22; //taken from man file
  int calculatedSum = 0;
  for(int i=0; i< 5; i++)
    calculatedSum += ((albaVMELandmarkCloud *)vmeRawMotionData->GetChild(i))->GetNumberOfLandmarks();

  CPPUNIT_ASSERT(calculatedSum == totalPoints);

  // test a coordinate in at first time step of a landmark to accept the validity of the reader
  // In this case I can say that this reader works correctly like a time-variant vme

  // IASL	IPE at time step 1
  double x,y,z;
  x = -390.13;
  y = 761.53;
  z = -289.80;
  
  ((albaVMELandmarkCloud *)vmeRawMotionData->GetChild(0))->TestModeOn();
  albaVMELandmark *landmark = ((albaVMELandmark *)((albaVMELandmarkCloud *)vmeRawMotionData->GetChild(0))->GetLandmark(0));
  

  albaString nameLand = landmark->GetName();
  CPPUNIT_ASSERT(nameLand == "IASR");

  double xyz[3];
  double rot[3];
  landmark->GetOutput()->GetPose(xyz , rot , 1);

  CPPUNIT_ASSERT(xyz[0] <= -390.12 && xyz[0] >= -390.14 &&
                 xyz[1] >= 761.52 &&  xyz[1] <= 761.54 &&
                 xyz[2] <= -289.79 && xyz[2] >= -289.81);
  

  // destroy vme
  albaDEL(vmeRawMotionData);

}
  
