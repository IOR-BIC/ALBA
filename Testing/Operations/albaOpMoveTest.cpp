/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMoveTest
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
#include "albaOpMoveTest.h"
#include "albaOpMove.h"

#include "albaVMEGroup.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEPolylineGraph.h"
#include "albaVMESurface.h"
#include "vtkALBASmartPointer.h"
#include "albaVMERoot.h"
#include "albaVME.h"
#include "albaVMEExternalData.h"

#include "vtkSphereSource.h"
#include "vtkDataSet.h"
//#include "vtkCellData.h"
//#include "vtkPolyData.h"

//-----------------------------------------------------------
void albaOpMoveTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpMove *move = new albaOpMove();
  albaDEL(move);
}
//-----------------------------------------------------------
void albaOpMoveTest::TestAccept() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaVMEPolylineGraph> polyline;
  albaSmartPointer<albaVMESurface> surface;
  albaSmartPointer<albaVMERoot> root;
  albaSmartPointer<albaVMEExternalData> external;

  albaOpMove *move = new albaOpMove();
  move->TestModeOn();

  // Try with accepted data type
  CPPUNIT_ASSERT( move->Accept(group) );
  CPPUNIT_ASSERT( move->Accept(volume) );
  CPPUNIT_ASSERT( move->Accept(polyline) );
  CPPUNIT_ASSERT( move->Accept(surface) );

  // Try with unwanted data type
  CPPUNIT_ASSERT( !move->Accept(root) );
  CPPUNIT_ASSERT( !move->Accept(external) );

  albaDEL(move);
}
//-----------------------------------------------------------
void albaOpMoveTest::TestReset() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  albaMatrix* old_matrix;
  albaNEW(old_matrix);
  old_matrix->DeepCopy((albaMatrix*)surface->GetOutput()->GetAbsMatrix());
  double value = old_matrix->GetElement(0,0);  

  albaOpMove *move = new albaOpMove();
  move->TestModeOn();
  move->SetInput(surface);
  move->OpRun();

  surface->GetOutput()->GetAbsMatrix()->SetElement(0,0,value*2);
  CPPUNIT_ASSERT( !((albaMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

  move->Reset();  
  CPPUNIT_ASSERT( ((albaMatrix*)surface->GetOutput()->GetAbsMatrix())->Equals(old_matrix) );

  albaDEL(old_matrix);
  albaDEL(move);
}