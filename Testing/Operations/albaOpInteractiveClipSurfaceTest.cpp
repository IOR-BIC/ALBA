/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpInteractiveClipSurfaceTest
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

#include "albaOpInteractiveClipSurfaceTest.h"
#include "albaOpInteractiveClipSurface.h"

#include "albaVMESurface.h"
#include "albaAbsMatrixPipe.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpInteractiveClipSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpInteractiveClipSurface *clip = new albaOpInteractiveClipSurface();
  albaDEL(clip);
}
//-----------------------------------------------------------
void albaOpInteractiveClipSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();
  vtkAppendPolyData *append = vtkAppendPolyData::New();
  append->SetInputConnection(sphere->GetOutputPort());
  append->Update();

  albaVMESurface *surfaceInput;
  albaNEW(surfaceInput);
  surfaceInput->SetData(append->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsBefore=append->GetOutput()->GetNumberOfPoints();
  CPPUNIT_ASSERT(numberOfPointsBefore==50);

  albaOpInteractiveClipSurface *clip = new albaOpInteractiveClipSurface();
  clip->TestModeOn();
  clip->SurfaceAccept(surfaceInput);
  clip->SetInput(surfaceInput);
  clip->OpRun();
  int result = clip->Clip();
  CPPUNIT_ASSERT(result==ALBA_OK);

  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsAfter=surfaceInput->GetOutput()->GetVTKData()->GetNumberOfPoints();
  CPPUNIT_ASSERT(numberOfPointsAfter==57);

  albaDEL(surfaceInput);

  append->Delete();
  sphere->Delete();

  albaDEL(clip);
}