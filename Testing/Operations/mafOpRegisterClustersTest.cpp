/*=========================================================================

 Program: MAF2
 Module: mafOpRegisterClustersTest
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
#include "mafOpRegisterClustersTest.h"
#include "mafOpRegisterClusters.h"

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();
  CPPUNIT_ASSERT(op != NULL);
  cppDEL(op);
}

// //----------------------------------------------------------------------------
// void mafOpRegisterClustersTest::OnEventTest()
// //----------------------------------------------------------------------------
// {
// 
// }

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::CopyTest()
//----------------------------------------------------------------------------
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();
  mafOp *op_copy = op->Copy();
  CPPUNIT_ASSERT(op_copy != NULL);
  CPPUNIT_ASSERT(op_copy->IsA("mafOpRegisterClusters"));

  cppDEL(op);
  cppDEL(op_copy);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::AcceptTest()
//----------------------------------------------------------------------------
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();
  CPPUNIT_ASSERT(op->Accept(NULL) == false);

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  int id = lmc->AppendLandmark(2,2,2,"2");

//   lmc->Open();
// 
//   // Do not accept opened landmark clouds
//   CPPUNIT_ASSERT(op->Accept(lmc) == false);


  // Accept closed not time varying landmark clouds
  CPPUNIT_ASSERT(op->Accept(lmc) == true);

  lmc->SetLandmark(id,2,2,2,1);

  // Do not accept closed time varying landmark clouds
  CPPUNIT_ASSERT(op->Accept(lmc) == false);

  mafDEL(lmc);

  cppDEL(op);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::OpRunTest()
//----------------------------------------------------------------------------
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  lmc->AppendLandmark(2,2,2,"2");

  op->TestModeOn();
  op->SetInput(lmc);
  op->OpRun();

  cppDEL(op);
  mafDEL(lmc);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::OpDoUndoTest()
//----------------------------------------------------------------------------
{
  mafOpRegisterClusters *op = new mafOpRegisterClusters();

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  lmc->AppendLandmark(2,2,2,"2");

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
void mafOpRegisterClustersTest::ClosedCloudAcceptTest()
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);

  lmc->AppendLandmark(0,0,0,"0");
  lmc->AppendLandmark(1,1,1,"1");
  lmc->AppendLandmark(2,2,2,"2");

  // Accept closed not time varying landmark clouds
  CPPUNIT_ASSERT(mafOpRegisterClusters::LMCloudAccept(lmc) == true);

  mafDEL(lmc);
}

//----------------------------------------------------------------------------
void mafOpRegisterClustersTest::SurfaceAcceptTest()
//----------------------------------------------------------------------------
{
  CPPUNIT_ASSERT(mafOpRegisterClusters::SurfaceAccept(NULL) == false);

  mafVMESurface *surf;
  mafNEW(surf);
  CPPUNIT_ASSERT(mafOpRegisterClusters::SurfaceAccept(surf) == true);
  mafDEL(surf);

  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);
  CPPUNIT_ASSERT(mafOpRegisterClusters::SurfaceAccept(lmc) == false);
  mafDEL(lmc);
}