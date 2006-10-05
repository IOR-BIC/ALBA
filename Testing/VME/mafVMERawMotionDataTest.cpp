/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMERawMotionDataTest.cpp,v $
Language:  C++
Date:      $Date: 2006-10-05 08:49:52 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVMERawMotionDataTest.h"
#include <iostream>

#include "mafVMERawMotionData.h"
#include "mafVMELandmarkCloud.h"
#include "mafDataVector.h"
#include "mafVMELandmark.h"


void mafVMERawMotionDataTest::Test()
{
  mafVMERawMotionData *vmeRawMotionData;

  // create reader
  mafNEW(vmeRawMotionData);

  // try to set this data to the volume
  int returnValue = 1;

  mafString filename=MAF_DATA_ROOT;
  filename<<"/RAW_MAL/Fprg3bsi_NOT_USED.man";

  mafString fileDict=MAF_DATA_ROOT;
  fileDict<<"/RAW_MAL/DICT_FPRG3BSL_421_NOT_USED.TXT";

  vmeRawMotionData->DictionaryOn();

  vmeRawMotionData->SetFileName(filename);
	vmeRawMotionData->SetDictionaryFileName(fileDict);

  // read control
  returnValue = vmeRawMotionData->Read();
  CPPUNIT_ASSERT(returnValue == MAF_OK);  // 0 if read is ok

  /*returnValue = -1;
  vmeRawMotionData->DictionaryOff();
  returnValue = vmeRawMotionData->Read();
  CPPUNIT_ASSERT(returnValue == MAF_OK);*/

  //child control
  long n = vmeRawMotionData->GetNumberOfChildren();
  CPPUNIT_ASSERT(n == 5); // 5 is the number of children taken from the typology in the dictionary

  int resultNameChild = -5;

  for(int i = 0; i < vmeRawMotionData->GetNumberOfChildren(); i++)
  {
    mafString name = vmeRawMotionData->GetChild(i)->GetName();
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
    calculatedSum += ((mafVMELandmarkCloud *)vmeRawMotionData->GetChild(i))->GetNumberOfLandmarks();

  CPPUNIT_ASSERT(calculatedSum == totalPoints);

  // test a coordinate in at first time step of a landmark to accept the validity of the reader
  // In this case I can say that this reader works correctly like a time-variant vme

  // IASL	IPE at time step 1
  double x,y,z;
  x = -390.13;
  y = 761.53;
  z = -289.80;
           
  ((mafVMELandmarkCloud *)vmeRawMotionData->GetChild(0))->Open();
  mafVMELandmark *landmark = ((mafVMELandmark *)((mafVMELandmarkCloud *)vmeRawMotionData->GetChild(0))->GetLandmark(0));
  

  mafString nameLand = landmark->GetName();
  CPPUNIT_ASSERT(nameLand == "IASR");

  double xyz[3];
  double rot[3];
  landmark->GetOutput()->GetPose(xyz , rot , 1);

  CPPUNIT_ASSERT(xyz[0] <= -390.12 && xyz[0] >= -390.14 &&
                 xyz[1] >= 761.52 &&  xyz[1] <= 761.54 &&
                 xyz[2] <= -289.79 && xyz[2] >= -289.81);
  

  // destroy vme
  mafDEL(vmeRawMotionData);

}
  
