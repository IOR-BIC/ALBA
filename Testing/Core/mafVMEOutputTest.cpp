/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEOutputTest.cpp,v $
Language:  C++
Date:      $Date: 2008-03-27 08:15:05 $
Version:   $Revision: 1.1 $
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
#include "mafVMEOutputTest.h"

#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEOutputVTK.h"
#include "mafVMEFactory.H"
#include "mafVMEStorage.h"
#include "mafVMERoot.h" 
#include "mafVMEVector.h"
#include "mafMatrix.h"
#include "mafSmartPointer.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkDataSet.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafVMEOutputTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputVTK *output = mafVMEOutputVTK::New();
  mafDEL(output);
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestSetVME()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface;
  mafVMESurface *surface1;
  mafNEW(surface);
  mafNEW(surface1);
  surface->SetName("surface");
  surface1->SetName("surface1");
  mafVMEOutput *vme_out = surface->GetOutput();
  vme_out->SetVME((mafVME*)surface1);

  mafVME *vme = vme_out->GetVME();

   //Test if GetVME() returned the VME set with SetVME()
  result = ((mafVMESurface*)vme)->Equals(surface1);
  TEST_RESULT;

  result = !((mafVMESurface*)vme)->Equals(surface);
  TEST_RESULT;

  mafDEL(surface);
  mafDEL(surface1);
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetVME()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface;
  mafNEW(surface);
  mafVMEOutput *vme_out = surface->GetOutput();

  mafVME *vme = vme_out->GetVME();

  //Test if GetVME() returned the right VME
  result = ((mafVMESurface*)vme)->Equals(surface);
  TEST_RESULT;
  mafDEL(surface);
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetPose()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface;
  mafNEW(surface);

  double x,y,z,rx,ry,rz;
  mafTimeStamp t;
  x = 0;
  y = 1;
  z = 2;
  rx = 3;
  ry = 4;
  rz = 5;
  t = 6;
  surface->SetPose(x,y,z,rx,ry,rz,t);
  mafVMEOutput *vme_out = surface->GetOutput();

  double x1,y1,z1,rx1,ry1,rz1;
   vme_out->GetPose(x1,y1,z1,rx1,ry1,rz1,t);

  //Test if GetPose() returned the right pose
  result = (fabs(x - x1) < 0.01  && fabs(y - y1) < 0.01 && fabs(z - z1) < 0.01 && 
            fabs(rx - rx1) < 0.01 && fabs(ry - ry1) < 0.01 && fabs(rz - rz1) < 0.01);
  TEST_RESULT;
  mafDEL(surface);
 
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetMatrix()
//----------------------------------------------------------------------------
{
  // Matrix pose for VME
  mafMatrix m;
  m.SetElement(0,3,3.0);
  m.SetTimeStamp(1.0);

  mafVMESurface *surface;
  mafNEW(surface);

  surface->SetMatrix(m);
  surface->SetTimeStamp(1.0);
  mafVMEOutput *vme_out = surface->GetOutput();

  //Test if GetMatrix() returned the right mafMatrix
  result = vme_out->GetMatrix()->Equals(&m);
  TEST_RESULT;

  surface->SetTimeStamp(0);
  mafMatrix mOut;
  vme_out->GetMatrix(mOut,1.0);

  //Test if GetMatrix() returned the right mafMatrix
  result = mOut.Equals(&m);
  TEST_RESULT;

  mafDEL(surface);
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetAbsMatrix()
//----------------------------------------------------------------------------
{
  // Matrix pose for VME
  mafMatrix m;
  m.SetElement(0,3,3.0);
  m.SetTimeStamp(1.0);

  mafVMESurface *surface;
  mafNEW(surface);

  surface->SetAbsMatrix(m);
  surface->SetTimeStamp(1.0);
  mafVMEOutput *vme_out = surface->GetOutput();

  //Test if GetAbsMatrix() returned the right mafMatrix
  result = vme_out->GetAbsMatrix()->Equals(&m);
  TEST_RESULT;

  surface->SetTimeStamp(0);
  mafMatrix mOut;
  vme_out->GetAbsMatrix(mOut,1.0);

  //Test if GetAbsMatrix() returned the right mafMatrix
  result = mOut.Equals(&m);
  TEST_RESULT;

  mafDEL(surface); 
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetAbsPose()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface;
  mafNEW(surface);

  double x,y,z,rx,ry,rz;
  mafTimeStamp t;
  x = 0;
  y = 1;
  z = 2;
  rx = 3;
  ry = 4;
  rz = 5;
  t = 6;
  surface->SetAbsPose(x,y,z,rx,ry,rz,t);
  mafVMEOutput *vme_out = surface->GetOutput();

  double x1,y1,z1,rx1,ry1,rz1;
  vme_out->GetAbsPose(x1,y1,z1,rx1,ry1,rz1,t);

  //Test if GetAbsPose() returned the right AbsPose
  result = mafEquals(x,x1)   && mafEquals(y,y1)   && mafEquals(z,z1) &&
           mafEquals(rx,rx1) && mafEquals(ry,ry1) && mafEquals(rz,rz1);
  TEST_RESULT;
  mafDEL(surface);
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface;
  mafNEW(surface);

  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_VMEOutput/sphere.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(filename.GetCStr());
  reader->Update();
  vtkDataSet *dataset = (vtkDataSet*)reader->GetOutput();

  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->Update();
  
  mafVMEOutput *vme_out = surface->GetOutput();
  vtkDataSet *datasetOut = vme_out->GetVTKData();

  double bounds[6];
  double boundsOut[6];
  datasetOut->GetBounds(bounds);
  datasetOut->GetBounds(boundsOut);

  //Test if GetVTKData() returned the right vtkDataSet by comparing bounds
  for (int i = 0; i < 6; i++)
  {
    result = mafEquals(bounds[i], boundsOut[i]);
    TEST_RESULT;
  }

 mafDEL(surface);
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetTimeBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *rootNode = node->FindInTreeByName("root");

  mafVMERoot *root = mafVMERoot::SafeDownCast(rootNode);

  double tbounds[2];
  root->GetOutput()->GetTimeBounds(tbounds);

  //Test if GetTimeBounds() returned the right time bounds
  result =  mafEquals(tbounds[0],0) && mafEquals(tbounds[1],4.58);
  TEST_RESULT;  
  mafDEL(storage);

}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetlocalTimeBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *vectorNode = node->FindInTreeByName("test_vector_GRFtimevar");

  mafVMEVector *vector = mafVMEVector::SafeDownCast(vectorNode);

  double tbounds[2];
  vector->GetOutput()->GetLocalTimeBounds(tbounds);

  //Test if GetLocalTimeBounds() returned the right time bounds
  result =  mafEquals(tbounds[0], 0) && mafEquals(tbounds[1], 4.58);
  TEST_RESULT;  
  mafDEL(storage);
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *rootNode = node->FindInTreeByName("root");

  mafVMERoot *surface = mafVMERoot::SafeDownCast(rootNode);

  mafVMEOutput *vme_out = surface->GetOutput();
  double bounds[6];
  vme_out->GetBounds(bounds);

  //Test if GetBounds() returned the right time bounds
  result = mafEquals(bounds[0], -1) && mafEquals(bounds[1], 240.821) && mafEquals(bounds[2], -2) && 
           mafEquals(bounds[3], 393.69448852539062) && mafEquals(bounds[4], -105.2) && mafEquals(bounds[5], 819.33654785156250);
  TEST_RESULT;

  mafDEL(storage);
}

//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetVMEBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *vectorNode = node->FindInTreeByName("test_vector_GRFtimevar");

  mafVMEVector *vector = mafVMEVector::SafeDownCast(vectorNode);

  mafVMEOutput *vme_out = vector->GetOutput();
  double bounds[6];
  vme_out->GetVMEBounds(bounds);

  //Test if GetVMEBounds() returned the right time bounds
  result = mafEquals(bounds[0], 17.284990310668949) && mafEquals(bounds[1], 79.174476623535156) && mafEquals(bounds[2], 324.78594970703131) && 
           mafEquals(bounds[3], 393.69448852539062) && mafEquals(bounds[4], 0) && mafEquals(bounds[5], 819.33654785156250);

  TEST_RESULT;

  mafDEL(storage);
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetVMELocalBounds()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_VMEOutput/Test_MSFImporter.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *vectorNode = node->FindInTreeByName("test_vector_GRFtimevar");

  mafVMEVector *vector = mafVMEVector::SafeDownCast(vectorNode);

  mafVMEOutput *vme_out = vector->GetOutput();
  double bounds[6];
  vme_out->GetVMELocalBounds(bounds);

  //Test if GetVMELocalBounds() returned the right time bounds
  result = mafEquals(bounds[0], 17.284990310668949) && mafEquals(bounds[1], 79.174476623535156) && mafEquals(bounds[2], 324.78594970703131) && 
           mafEquals(bounds[3], 393.69448852539062) && mafEquals(bounds[4], 0) && mafEquals(bounds[5], 819.33654785156250);
  TEST_RESULT;

  mafDEL(storage);
}
//----------------------------------------------------------------------------
void mafVMEOutputTest::TestGetTimeStamp()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface;
  mafNEW(surface);

  surface->SetTimeStamp(1.0);
  mafVMEOutput *vme_out = surface->GetOutput();

  //Test if GetTimeStamp() returned the right time stamp
  result = vme_out->GetTimeStamp() == 1;
  TEST_RESULT;

  mafDEL(surface);
}
