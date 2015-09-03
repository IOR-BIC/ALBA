/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputSurfaceEditorTest
 Authors: Daniele Giunchi
 
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
#include "mafVMEOutputSurfaceEditorTest.h"

#include "mafVMEOutputSurfaceEditor.h"

#include "mafVMESurfaceEditor.h"


#include "mafMatrix.h"
#include "vtkPolyData.h"
#include "vtkCubeSource.h"

#include "vtkPointData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void mafVMEOutputSurfaceEditorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceEditorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputSurfaceEditor outputSurface;
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceEditorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputSurfaceEditor *outputSurface = new mafVMEOutputSurfaceEditor();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceEditorTest::TestGetSurfaceData_Update()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  mafVMESurfaceEditor *vmeSurfaceEditor;
  mafNEW(vmeSurfaceEditor);
  
  vtkCubeSource *source;
  vtkNEW(source);
  source->Update();
  vmeSurfaceEditor->SetData(source->GetOutput(), 0.0);

  mafVMEOutputSurfaceEditor *outputSurfaceEditor = NULL;
  outputSurfaceEditor = mafVMEOutputSurfaceEditor::SafeDownCast(vmeSurfaceEditor->GetOutput());
  outputSurfaceEditor->Update();
  m_Result = outputSurfaceEditor != NULL && 
             vmeSurfaceEditor->GetOutput()->GetVTKData() == outputSurfaceEditor->GetSurfaceData();

  TEST_RESULT;

  vtkDEL(source);
  mafDEL(vmeSurfaceEditor);
}