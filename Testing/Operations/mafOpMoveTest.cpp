/*=========================================================================

 Program: MAF2Medical
 Module: mafOpMoveTest
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
#include "mafOpMoveTest.h"
#include "mafOpMove.h"

#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "medVMEPolylineGraph.h"
#include "mafVMESurface.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMERoot.h"
#include "mafVME.h"
#include "mafVMEExternalData.h"

#include "vtkSphereSource.h"
#include "vtkDataSet.h"
//#include "vtkCellData.h"
//#include "vtkPolyData.h"

//-----------------------------------------------------------
void mafOpMoveTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpMove *move = new mafOpMove();
  mafDEL(move);
}
//-----------------------------------------------------------
void mafOpMoveTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<medVMEPolylineGraph> polyline;
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMERoot> root;
  mafSmartPointer<mafVMEExternalData> external;

  mafOpMove *move = new mafOpMove();
  move->TestModeOn();

  // Try with accepted data type
  CPPUNIT_ASSERT( move->Accept(group) );
  CPPUNIT_ASSERT( move->Accept(volume) );
  CPPUNIT_ASSERT( move->Accept(polyline) );
  CPPUNIT_ASSERT( move->Accept(surface) );

  // Try with unwanted data type
  CPPUNIT_ASSERT( !move->Accept(root) );
  CPPUNIT_ASSERT( !move->Accept(external) );

  mafDEL(move);
}
//-----------------------------------------------------------
void mafOpMoveTest::TestReset() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

  mafMatrix* old_matrix;
  mafNEW(old_matrix);
  old_matrix->DeepCopy((mafMatrix*)surface->GetOutput()->GetAbsMatrix());
  double value = old_matrix->GetElement(0,0);  

  mafOpMove *move = new mafOpMove();
  move->TestModeOn();
  move->SetInput(surface);
  move->OpRun();

  surface->GetOutput()->GetAbsMatrix()->SetElement(0,0,value*2);
  CPPUNIT_ASSERT( !((mafMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

  move->Reset();  
  CPPUNIT_ASSERT( ((mafMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

  mafDEL(old_matrix);
  mafDEL(move);
}