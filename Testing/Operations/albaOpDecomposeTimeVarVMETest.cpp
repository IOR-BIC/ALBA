/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpDecomposeTimeVarVMETest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaOpDecomposeTimeVarVMETest.h"

#include "albaOpDecomposeTimeVarVME.h"

#include "albaVMEFactory.h"
#include "albaVMEGenericAbstract.h"
#include "albaVMERoot.h"
#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaMatrix.h"
#include "albaMatrixVector.h"
#include "albaVMEStorage.h"

void albaOpDecomposeTimeVarVMETest::TestFixture()
{
}

//-----------------------------------------------------------
void albaOpDecomposeTimeVarVMETest::TestConstructor()
//-----------------------------------------------------------
{
  albaOpDecomposeTimeVarVME *decompose = new albaOpDecomposeTimeVarVME("decompose vme");
  albaDEL(decompose);
}

//-----------------------------------------------------------
void albaOpDecomposeTimeVarVMETest::TestModeTimeStampsVME()
//-----------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);
 
  albaVME *node = storage->GetRoot();
  albaVME *oldNode = node->FindInTreeByName("C7");

  albaOpDecomposeTimeVarVME *decompose = new albaOpDecomposeTimeVarVME("decompose vme");
  decompose->TestModeOn();
  decompose->SetInput(oldNode );

  char *frame = "2.5000";
  decompose->AppendFrame(frame);
  decompose->SelectMode(albaOpDecomposeTimeVarVME::MODE_FRAMES);
  decompose->UpdateFrames();

  albaVME *decomposedNode = decompose->GetOutput();
  albaVME *newNode = decomposedNode->FindInTreeByName("C7_2.500");

  CPPUNIT_ASSERT(newNode->IsA("albaVMELandmark"));

  albaVMELandmark *oldLandmark = albaVMELandmark::SafeDownCast(oldNode);
  albaVMELandmark *newLandmark = albaVMELandmark::SafeDownCast(newNode);

  CPPUNIT_ASSERT(newLandmark->GetNumberOfTimeStamps() == 1);

  double oldXyz[3];
  double newXyz[3];
  oldLandmark->GetPoint(oldXyz, 2.5);
  newLandmark->GetPoint(newXyz, 0);
  CPPUNIT_ASSERT(oldXyz[0] == newXyz[0] && oldXyz[1] == newXyz[1] && oldXyz[2] == newXyz[2]);

  albaVMEGenericAbstract *vmeGenericOld = albaVMEGenericAbstract::SafeDownCast(oldLandmark);
  albaVMEGenericAbstract *vmeGenericNew = albaVMEGenericAbstract::SafeDownCast(newLandmark);

  albaTimeStamp frameTime = atof(frame);
  albaMatrixVector *mvOld =  vmeGenericOld->GetMatrixVector();
  albaMatrix *matrixOld = mvOld->GetNearestMatrix(frameTime);

  albaMatrixVector *mvNew =  vmeGenericNew->GetMatrixVector();
  albaMatrix *matrixNew = mvNew->GetNearestMatrix(frameTime);

  CPPUNIT_ASSERT(matrixNew->Equals(matrixOld));

  albaDEL(decompose);
  albaDEL(storage);
}

//-----------------------------------------------------------
void albaOpDecomposeTimeVarVMETest::TestModeIntervalVME()
//-----------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *oldNode = node->FindInTreeByName("C7");

  albaOpDecomposeTimeVarVME *decompose = new albaOpDecomposeTimeVarVME("decompose vme");
  decompose->TestModeOn();
  decompose->SetInput(oldNode );

  double from = 2.500;
  double to = 2.520;
  decompose->SelectMode(albaOpDecomposeTimeVarVME::MODE_INTERVAL);
  decompose->SetInterval(from, to);
  decompose->UpdateFrames();

  albaVME *decomposedNode = decompose->GetOutput();
  albaVME *newNode0 = decomposedNode->FindInTreeByName("C7_2.500");
  albaVME *newNode1= decomposedNode->FindInTreeByName("C7_2.510");
  albaVME *newNode2 = decomposedNode->FindInTreeByName("C7_2.520");

  CPPUNIT_ASSERT(newNode0->IsA("albaVMELandmark"));
  CPPUNIT_ASSERT(newNode1->IsA("albaVMELandmark"));
  CPPUNIT_ASSERT(newNode2->IsA("albaVMELandmark"));

  albaVMELandmark *oldLandmark = (albaVMELandmark *)oldNode;
  albaVMELandmark *newLandmark0 = (albaVMELandmark *)newNode0;
  albaVMELandmark *newLandmark1 = (albaVMELandmark *)newNode1;
  albaVMELandmark *newLandmark2 = (albaVMELandmark *)newNode2;

  CPPUNIT_ASSERT(newLandmark0->GetNumberOfTimeStamps() == 1);
  CPPUNIT_ASSERT(newLandmark1->GetNumberOfTimeStamps() == 1);
  CPPUNIT_ASSERT(newLandmark2->GetNumberOfTimeStamps() == 1);

  double oldXyz[3];
  double newXyz[3];
  oldLandmark->GetPoint(oldXyz, 2.5);
  newLandmark0->GetPoint(newXyz, 0);
  CPPUNIT_ASSERT(oldXyz[0] == newXyz[0] && oldXyz[1] == newXyz[1] && oldXyz[2] == newXyz[2]);

  oldLandmark->GetPoint(oldXyz, 2.51);
  newLandmark1->GetPoint(newXyz, 0);
  CPPUNIT_ASSERT(oldXyz[0] == newXyz[0] && oldXyz[1] == newXyz[1] && oldXyz[2] == newXyz[2]);

  oldLandmark->GetPoint(oldXyz, 2.52);
  newLandmark2->GetPoint(newXyz, 0);
  CPPUNIT_ASSERT(oldXyz[0] == newXyz[0] && oldXyz[1] == newXyz[1] && oldXyz[2] == newXyz[2]);

  albaDEL(decompose);
  albaDEL(storage);
}

//-----------------------------------------------------------
void albaOpDecomposeTimeVarVMETest::TestModePeriodicityVME()
//-----------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *oldNode = node->FindInTreeByName("C7");

  albaOpDecomposeTimeVarVME *decompose = new albaOpDecomposeTimeVarVME("decompose vme");
  decompose->TestModeOn();
  decompose->SetInput(oldNode );

  decompose->SelectMode(albaOpDecomposeTimeVarVME::MODE_PERIODICITY);
  decompose->SetPeriodicity(50);
  decompose->UpdateFrames();

  albaVME *decomposedNode = decompose->GetOutput();
  albaVMELandmark *oldLandmark = albaVMELandmark::SafeDownCast(oldNode);

  int timeNumber = oldLandmark->GetNumberOfTimeStamps();
  bool rightNumber = decomposedNode->GetNumberOfChildren() == timeNumber/50;
  CPPUNIT_ASSERT(rightNumber);

  albaDEL(decompose);
  albaDEL(storage);
}
