/*=========================================================================

 Program: MAF2Medical
 Module: medOpRegisterClustersTest
 Authors: Alberto Losi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "medOpRegisterClustersTest.h"
#include "medOpRegisterClusters.h"

//----------------------------------------------------------------------------
void medOpRegisterClustersTest::setUp()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void medOpRegisterClustersTest::tearDown()
//----------------------------------------------------------------------------
{
  delete wxLog::SetActiveTarget(NULL);
}

//----------------------------------------------------------------------------
void medOpRegisterClustersTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  medOpRegisterClusters *op = new medOpRegisterClusters();
  CPPUNIT_ASSERT(op != NULL);
  cppDEL(op);
}

// //----------------------------------------------------------------------------
// void medOpRegisterClustersTest::OnEventTest()
// //----------------------------------------------------------------------------
// {
// 
// }

//----------------------------------------------------------------------------
void medOpRegisterClustersTest::CopyTest()
//----------------------------------------------------------------------------
{
  medOpRegisterClusters *op = new medOpRegisterClusters();
  mafOp *op_copy = op->Copy();
  CPPUNIT_ASSERT(op_copy != NULL);
  CPPUNIT_ASSERT(op_copy->IsA("medOpRegisterClusters"));

  cppDEL(op);
  cppDEL(op_copy);
}

//----------------------------------------------------------------------------
void medOpRegisterClustersTest::AcceptTest()
//----------------------------------------------------------------------------
{
  medOpRegisterClusters *op = new medOpRegisterClusters();
  CPPUNIT_ASSERT(op->Accept(NULL) == false);

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0",false);
  lmc->AppendLandmark(1,1,1,"1",false);
  int id = lmc->AppendLandmark(2,2,2,"2",false);

//   lmc->Open();
// 
//   // Do not accept opened landmark clouds
//   CPPUNIT_ASSERT(op->Accept(lmc) == false);

  lmc->Close();

  // Accept closed not time varying landmark clouds
  CPPUNIT_ASSERT(op->Accept(lmc) == true);

  lmc->SetLandmark(id,2,2,2,1);

  // Do not accept closed time varying landmark clouds
  CPPUNIT_ASSERT(op->Accept(lmc) == false);

  mafDEL(lmc);

  cppDEL(op);
}

//----------------------------------------------------------------------------
void medOpRegisterClustersTest::OpRunTest()
//----------------------------------------------------------------------------
{
  medOpRegisterClusters *op = new medOpRegisterClusters();

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0",false);
  lmc->AppendLandmark(1,1,1,"1",false);
  lmc->AppendLandmark(2,2,2,"2",false);

  lmc->Close();

  op->TestModeOn();
  op->SetInput(lmc);
  op->OpRun();

  cppDEL(op);
  mafDEL(lmc);
}

//----------------------------------------------------------------------------
void medOpRegisterClustersTest::OpDoUndoTest()
//----------------------------------------------------------------------------
{
  medOpRegisterClusters *op = new medOpRegisterClusters();

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0",false);
  lmc->AppendLandmark(1,1,1,"1",false);
  lmc->AppendLandmark(2,2,2,"2",false);

  lmc->Close();

  op->TestModeOn();
  op->SetInput(lmc);

  op->OpRun();

  mafVMESurface *surf;
  mafNEW(surf);

  op->SetFollower(surf);
  op->SetTarget(lmc);


  op->OpDo();

  CPPUNIT_ASSERT(op->GetResult() != NULL);

  op->OpUndo();

  CPPUNIT_ASSERT(op->GetResult() == NULL);

  cppDEL(op);
  mafDEL(lmc);
  mafDEL(surf);
}

//----------------------------------------------------------------------------
void medOpRegisterClustersTest::ClosedCloudAcceptTest()
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0",false);
  lmc->AppendLandmark(1,1,1,"1",false);
  lmc->AppendLandmark(2,2,2,"2",false);

//   lmc->Open();
// 
//   // Do not accept opened landmark clouds
//   CPPUNIT_ASSERT(medOpRegisterClusters::ClosedCloudAccept(lmc) == false);

  lmc->Close();

  // Accept closed not time varying landmark clouds
  CPPUNIT_ASSERT(medOpRegisterClusters::ClosedCloudAccept(lmc) == true);

  mafDEL(lmc);
}

//----------------------------------------------------------------------------
void medOpRegisterClustersTest::SurfaceAcceptTest()
//----------------------------------------------------------------------------
{
  CPPUNIT_ASSERT(medOpRegisterClusters::SurfaceAccept(NULL) == false);

  mafVMESurface *surf;
  mafNEW(surf);
  CPPUNIT_ASSERT(medOpRegisterClusters::SurfaceAccept(surf) == true);
  mafDEL(surf);

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);
  CPPUNIT_ASSERT(medOpRegisterClusters::SurfaceAccept(lmc) == false);
  mafDEL(lmc);
}