/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpDecomposeTimeVarVMETest.cpp,v $
Language:  C++
Date:      $Date: 2011-07-11 14:10:53 $
Version:   $Revision: 1.1.4.1 $
Authors:   Roberto Mucci
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
#include "mafOpDecomposeTimeVarVMETest.h"

#include "mafOpDecomposeTimeVarVME.h"

#include "mafVMEFactory.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafMatrix.h"
#include "mafMatrixVector.h"
#include "mafVMEStorage.h"

void mafOpDecomposeTimeVarVMETest::TestFixture()
{
}

void mafOpDecomposeTimeVarVMETest::setUp()
{
}

void mafOpDecomposeTimeVarVMETest::tearDown()
{
}

//-----------------------------------------------------------
void mafOpDecomposeTimeVarVMETest::TestConstructor()
//-----------------------------------------------------------
{
  mafOpDecomposeTimeVarVME *decompose = new mafOpDecomposeTimeVarVME("decompose vme");
  mafDEL(decompose);
}

//-----------------------------------------------------------
void mafOpDecomposeTimeVarVMETest::TestModeTimeStampsVME()
//-----------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);
 
  mafNode *node = storage->GetRoot();
  mafNode *oldNode = node->FindInTreeByName("C7");

  mafOpDecomposeTimeVarVME *decompose = new mafOpDecomposeTimeVarVME("decompose vme");
  decompose->TestModeOn();
  decompose->SetInput(oldNode );

  char *frame = "2.5000";
  decompose->AppendFrame(frame);
  decompose->SelectMode(mafOpDecomposeTimeVarVME::MODE_FRAMES);
  decompose->UpdateFrames();

  mafNode *decomposedNode = decompose->GetOutput();
  mafNode *newNode = decomposedNode->FindInTreeByName("C7_2.500");

  CPPUNIT_ASSERT(newNode->IsA("mafVMELandmark"));

  mafVMELandmark *oldLandmark = mafVMELandmark::SafeDownCast(oldNode);
  mafVMELandmark *newLandmark = mafVMELandmark::SafeDownCast(newNode);

  CPPUNIT_ASSERT(newLandmark->GetNumberOfTimeStamps() == 1);

  double oldXyz[3];
  double newXyz[3];
  oldLandmark->GetPoint(oldXyz, 2.5);
  newLandmark->GetPoint(newXyz, 0);
  CPPUNIT_ASSERT(oldXyz[0] == newXyz[0] && oldXyz[1] == newXyz[1] && oldXyz[2] == newXyz[2]);

  mafVMEGenericAbstract *vmeGenericOld = mafVMEGenericAbstract::SafeDownCast(oldLandmark);
  mafVMEGenericAbstract *vmeGenericNew = mafVMEGenericAbstract::SafeDownCast(newLandmark);

  mafTimeStamp frameTime = atof(frame);
  mafMatrixVector *mvOld =  vmeGenericOld->GetMatrixVector();
  mafMatrix *matrixOld = mvOld->GetNearestMatrix(frameTime);

  mafMatrixVector *mvNew =  vmeGenericNew->GetMatrixVector();
  mafMatrix *matrixNew = mvNew->GetNearestMatrix(frameTime);

  CPPUNIT_ASSERT(matrixNew->Equals(matrixOld));

  mafDEL(decompose);
  mafDEL(storage);
}

//-----------------------------------------------------------
void mafOpDecomposeTimeVarVMETest::TestModeIntervalVME()
//-----------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *oldNode = node->FindInTreeByName("C7");

  mafOpDecomposeTimeVarVME *decompose = new mafOpDecomposeTimeVarVME("decompose vme");
  decompose->TestModeOn();
  decompose->SetInput(oldNode );

  double from = 2.500;
  double to = 2.520;
  decompose->SelectMode(mafOpDecomposeTimeVarVME::MODE_INTERVAL);
  decompose->SetInterval(from, to);
  decompose->UpdateFrames();

  mafNode *decomposedNode = decompose->GetOutput();
  mafNode *newNode0 = decomposedNode->FindInTreeByName("C7_2.500");
  mafNode *newNode1= decomposedNode->FindInTreeByName("C7_2.510");
  mafNode *newNode2 = decomposedNode->FindInTreeByName("C7_2.520");

  CPPUNIT_ASSERT(newNode0->IsA("mafVMELandmark"));
  CPPUNIT_ASSERT(newNode1->IsA("mafVMELandmark"));
  CPPUNIT_ASSERT(newNode2->IsA("mafVMELandmark"));

  mafVMELandmark *oldLandmark = (mafVMELandmark *)oldNode;
  mafVMELandmark *newLandmark0 = (mafVMELandmark *)newNode0;
  mafVMELandmark *newLandmark1 = (mafVMELandmark *)newNode1;
  mafVMELandmark *newLandmark2 = (mafVMELandmark *)newNode2;

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

  mafDEL(decompose);
  mafDEL(storage);
}

//-----------------------------------------------------------
void mafOpDecomposeTimeVarVMETest::TestModePeriodicityVME()
//-----------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_DecomposeTimeVarVME/DecomposeTimeVarVME_test.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *oldNode = node->FindInTreeByName("C7");

  mafOpDecomposeTimeVarVME *decompose = new mafOpDecomposeTimeVarVME("decompose vme");
  decompose->TestModeOn();
  decompose->SetInput(oldNode );

  decompose->SelectMode(mafOpDecomposeTimeVarVME::MODE_PERIODICITY);
  decompose->SetPeriodicity(50);
  decompose->UpdateFrames();

  mafNode *decomposedNode = decompose->GetOutput();
  mafVMELandmark *oldLandmark = mafVMELandmark::SafeDownCast(oldNode);

  int timeNumber = oldLandmark->GetNumberOfTimeStamps();
  bool rightNumber = decomposedNode->GetNumberOfChildren() == timeNumber/50;
  CPPUNIT_ASSERT(rightNumber);

  mafDEL(decompose);
  mafDEL(storage);
}
