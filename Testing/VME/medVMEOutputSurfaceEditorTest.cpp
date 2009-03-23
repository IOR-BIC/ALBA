/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEOutputSurfaceEditorTest.cpp,v $
Language:  C++
Date:      $Date: 2009-03-23 09:19:39 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
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
#include "medVMEOutputSurfaceEditorTest.h"

#include "medVMEOutputSurfaceEditor.h"

#include "medVMESurfaceEditor.h"


#include "mafMatrix.h"
#include "vtkPolyData.h"
#include "vtkCubeSource.h"

#include "vtkPointData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void medVMEOutputSurfaceEditorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medVMEOutputSurfaceEditorTest::setUp()
//----------------------------------------------------------------------------
{
	m_Result = false;
}
//----------------------------------------------------------------------------
void medVMEOutputSurfaceEditorTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medVMEOutputSurfaceEditorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  medVMEOutputSurfaceEditor outputSurface;
}
//----------------------------------------------------------------------------
void medVMEOutputSurfaceEditorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  medVMEOutputSurfaceEditor *outputSurface = new medVMEOutputSurfaceEditor();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void medVMEOutputSurfaceEditorTest::TestGetSurfaceData_Update()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  medVMESurfaceEditor *vmeSurfaceEditor;
  mafNEW(vmeSurfaceEditor);
  
  vtkCubeSource *source;
  vtkNEW(source);
  source->Update();
  vmeSurfaceEditor->SetData(source->GetOutput(), 0.0);

  medVMEOutputSurfaceEditor *outputSurfaceEditor = NULL;
  outputSurfaceEditor = medVMEOutputSurfaceEditor::SafeDownCast(vmeSurfaceEditor->GetOutput());
  outputSurfaceEditor->Update();
  m_Result = outputSurfaceEditor != NULL && 
             vmeSurfaceEditor->GetOutput()->GetVTKData() == outputSurfaceEditor->GetSurfaceData();

  TEST_RESULT;

  vtkDEL(source);
  mafDEL(vmeSurfaceEditor);
}