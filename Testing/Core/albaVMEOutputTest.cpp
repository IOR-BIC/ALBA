/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputTest
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
#include "albaVMEOutputTest.h"

#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaVMEOutputVTK.h"
#include "albaVMEFactory.H"
#include "albaVMEStorage.h"
#include "albaVMERoot.h" 
#include "albaVMEVector.h"
#include "albaMatrix.h"
#include "albaSmartPointer.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkDataSet.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaVMEOutputTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputVTK *output = albaVMEOutputVTK::New();
  albaDEL(output);
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestSetVME()
//----------------------------------------------------------------------------
{
  albaVMESurface *surface;
  albaVMESurface *surface1;
  albaNEW(surface);
  albaNEW(surface1);
  surface->SetName("surface");
  surface1->SetName("surface1");
  albaVMEOutput *vme_out = surface->GetOutput();
  vme_out->SetVME(surface1);

  albaVME *vme = vme_out->GetVME();

   //Test if GetVME() returned the VME set with SetVME()
  result = ((albaVMESurface*)vme)->Equals(surface1);
  TEST_RESULT;

  result = !((albaVMESurface*)vme)->Equals(surface);
  TEST_RESULT;

  albaDEL(surface);
  albaDEL(surface1);
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetVME()
//----------------------------------------------------------------------------
{
  albaVMESurface *surface;
  albaNEW(surface);
  albaVMEOutput *vme_out = surface->GetOutput();

  albaVME *vme = vme_out->GetVME();

  //Test if GetVME() returned the right VME
  result = ((albaVMESurface*)vme)->Equals(surface);
  TEST_RESULT;
  albaDEL(surface);
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetPose()
//----------------------------------------------------------------------------
{
  albaVMESurface *surface;
  albaNEW(surface);

  double x,y,z,rx,ry,rz;
  albaTimeStamp t;
  x = 0;
  y = 1;
  z = 2;
  rx = 3;
  ry = 4;
  rz = 5;
  t = 6;
  surface->SetPose(x,y,z,rx,ry,rz,t);
  albaVMEOutput *vme_out = surface->GetOutput();

  double x1,y1,z1,rx1,ry1,rz1;
   vme_out->GetPose(x1,y1,z1,rx1,ry1,rz1,t);

  //Test if GetPose() returned the right pose
  result = (fabs(x - x1) < 0.01  && fabs(y - y1) < 0.01 && fabs(z - z1) < 0.01 && 
            fabs(rx - rx1) < 0.01 && fabs(ry - ry1) < 0.01 && fabs(rz - rz1) < 0.01);
  TEST_RESULT;
  albaDEL(surface);
 
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetMatrix()
//----------------------------------------------------------------------------
{
  // Matrix pose for VME
  albaMatrix m;
  m.SetElement(0,3,3.0);
  m.SetTimeStamp(1.0);

  albaVMESurface *surface;
  albaNEW(surface);

  surface->SetMatrix(m);
  surface->SetTimeStamp(1.0);
  albaVMEOutput *vme_out = surface->GetOutput();

  //Test if GetMatrix() returned the right albaMatrix
  result = vme_out->GetMatrix()->Equals(&m);
  TEST_RESULT;

  surface->SetTimeStamp(0);
  albaMatrix mOut;
  vme_out->GetMatrix(mOut,1.0);

  //Test if GetMatrix() returned the right albaMatrix
  result = mOut.Equals(&m);
  TEST_RESULT;

  albaDEL(surface);
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetAbsMatrix()
//----------------------------------------------------------------------------
{
  // Matrix pose for VME
  albaMatrix m;
  m.SetElement(0,3,3.0);
  m.SetTimeStamp(1.0);

  albaVMESurface *surface;
  albaNEW(surface);

  surface->SetAbsMatrix(m);
  surface->SetTimeStamp(1.0);
  albaVMEOutput *vme_out = surface->GetOutput();

  //Test if GetAbsMatrix() returned the right albaMatrix
  result = vme_out->GetAbsMatrix()->Equals(&m);
  TEST_RESULT;

  surface->SetTimeStamp(0);
  albaMatrix mOut;
  vme_out->GetAbsMatrix(mOut,1.0);

  //Test if GetAbsMatrix() returned the right albaMatrix
  result = mOut.Equals(&m);
  TEST_RESULT;

  albaDEL(surface); 
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetAbsPose()
//----------------------------------------------------------------------------
{
  albaVMESurface *surface;
  albaNEW(surface);

  double x,y,z,rx,ry,rz;
  albaTimeStamp t;
  x = 0;
  y = 1;
  z = 2;
  rx = 3;
  ry = 4;
  rz = 5;
  t = 6;
  surface->SetAbsPose(x,y,z,rx,ry,rz,t);
  albaVMEOutput *vme_out = surface->GetOutput();

  double x1,y1,z1,rx1,ry1,rz1;
  vme_out->GetAbsPose(x1,y1,z1,rx1,ry1,rz1,t);

  //Test if GetAbsPose() returned the right AbsPose
  result = albaEquals(x,x1)   && albaEquals(y,y1)   && albaEquals(z,z1) &&
           albaEquals(rx,rx1) && albaEquals(ry,ry1) && albaEquals(rz,rz1);
  TEST_RESULT;
  albaDEL(surface);
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  albaVMESurface *surface;
  albaNEW(surface);

  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_VMEOutput/sphere.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(filename.GetCStr());
  reader->Update();
  vtkDataSet *dataset = (vtkDataSet*)reader->GetOutput();

  surface->SetData(reader->GetOutput(),0.0);
  
  albaVMEOutput *vme_out = surface->GetOutput();
  vtkDataSet *datasetOut = vme_out->GetVTKData();

  double bounds[6];
  double boundsOut[6];
  datasetOut->GetBounds(bounds);
  datasetOut->GetBounds(boundsOut);

  //Test if GetVTKData() returned the right vtkDataSet by comparing bounds
  for (int i = 0; i < 6; i++)
  {
    result = albaEquals(bounds[i], boundsOut[i]);
    TEST_RESULT;
  }

 albaDEL(surface);
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetTimeBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *rootNode = node->FindInTreeByName("root");

  albaVMERoot *root = albaVMERoot::SafeDownCast(rootNode);

  double tbounds[2];
  root->GetOutput()->GetTimeBounds(tbounds);

  //Test if GetTimeBounds() returned the right time bounds
  result =  albaEquals(tbounds[0],0) && albaEquals(tbounds[1],4.58);
  TEST_RESULT;  
  albaDEL(storage);

}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetlocalTimeBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *vectorNode = node->FindInTreeByName("test_vector_GRFtimevar");

  albaVMEVector *vector = albaVMEVector::SafeDownCast(vectorNode);

  double tbounds[2];
  vector->GetOutput()->GetLocalTimeBounds(tbounds);

  //Test if GetLocalTimeBounds() returned the right time bounds
  result =  albaEquals(tbounds[0], 0) && albaEquals(tbounds[1], 4.58);
  TEST_RESULT;  
  albaDEL(storage);
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *rootNode = node->FindInTreeByName("root");

  albaVMERoot *surface = albaVMERoot::SafeDownCast(rootNode);

  albaVMEOutput *vme_out = surface->GetOutput();
  double bounds[6];
  vme_out->GetBounds(bounds);

  //Test if GetBounds() returned the right time bounds
  result = albaEquals(bounds[0], -1) && albaEquals(bounds[1], 240.821) && albaEquals(bounds[2], -2) && 
           albaEquals(bounds[3], 393.69448852539062) && albaEquals(bounds[4], -105.2) && albaEquals(bounds[5], 819.33654785156250);
  TEST_RESULT;

  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetVMEBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *vectorNode = node->FindInTreeByName("test_vector_GRFtimevar");

  albaVMEVector *vector = albaVMEVector::SafeDownCast(vectorNode);

  albaVMEOutput *vme_out = vector->GetOutput();
  double bounds[6];
  vme_out->GetVMEBounds(bounds);

  //Test if GetVMEBounds() returned the right time bounds
  result = albaEquals(bounds[0], 17.284990310668949) && albaEquals(bounds[1], 79.174476623535156) && albaEquals(bounds[2], 324.78594970703131) && 
           albaEquals(bounds[3], 393.69448852539062) && albaEquals(bounds[4], 0) && albaEquals(bounds[5], 819.33654785156250);

  TEST_RESULT;

  albaDEL(storage);
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetVMELocalBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *vectorNode = node->FindInTreeByName("test_vector_GRFtimevar");

  albaVMEVector *vector = albaVMEVector::SafeDownCast(vectorNode);

  albaVMEOutput *vme_out = vector->GetOutput();
  double bounds[6];
  vme_out->GetVMELocalBounds(bounds);

  //Test if GetVMELocalBounds() returned the right time bounds
  result = albaEquals(bounds[0], 17.284990310668949) && albaEquals(bounds[1], 79.174476623535156) && albaEquals(bounds[2], 324.78594970703131) && 
           albaEquals(bounds[3], 393.69448852539062) && albaEquals(bounds[4], 0) && albaEquals(bounds[5], 819.33654785156250);
  TEST_RESULT;

  albaDEL(storage);
}
//----------------------------------------------------------------------------
void albaVMEOutputTest::TestGetTimeStamp()
//----------------------------------------------------------------------------
{
  albaVMESurface *surface;
  albaNEW(surface);

  surface->SetTimeStamp(1.0);
  albaVMEOutput *vme_out = surface->GetOutput();

  //Test if GetTimeStamp() returned the right time stamp
  result = vme_out->GetTimeStamp() == 1;
  TEST_RESULT;

  albaDEL(surface);
}
