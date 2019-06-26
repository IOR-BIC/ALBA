/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputSurfaceEditorTest
 Authors: Daniele Giunchi
 
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
#include "albaVMEOutputSurfaceEditorTest.h"

#include "albaVMEOutputSurfaceEditor.h"

#include "albaVMESurfaceEditor.h"


#include "albaMatrix.h"
#include "vtkPolyData.h"
#include "vtkCubeSource.h"

#include "vtkPointData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaVMEOutputSurfaceEditorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputSurfaceEditorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputSurfaceEditor outputSurface;
}
//----------------------------------------------------------------------------
void albaVMEOutputSurfaceEditorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputSurfaceEditor *outputSurface = new albaVMEOutputSurfaceEditor();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void albaVMEOutputSurfaceEditorTest::TestGetSurfaceData_Update()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  albaVMESurfaceEditor *vmeSurfaceEditor;
  albaNEW(vmeSurfaceEditor);
  
  vtkCubeSource *source;
  vtkNEW(source);
  source->Update();
  vmeSurfaceEditor->SetData(source->GetOutput(), 0.0);

  albaVMEOutputSurfaceEditor *outputSurfaceEditor = NULL;
  outputSurfaceEditor = albaVMEOutputSurfaceEditor::SafeDownCast(vmeSurfaceEditor->GetOutput());
  outputSurfaceEditor->Update();
  m_Result = outputSurfaceEditor != NULL && 
             vmeSurfaceEditor->GetOutput()->GetVTKData() == outputSurfaceEditor->GetSurfaceData();

  TEST_RESULT;

  vtkDEL(source);
  albaDEL(vmeSurfaceEditor);
}