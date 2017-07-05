/*=========================================================================

 Program: MAF2
 Module: mafOpInteractiveClipSurfaceTest
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

#include "mafOpInteractiveClipSurfaceTest.h"
#include "mafOpInteractiveClipSurface.h"

#include "mafVMESurface.h"
#include "mafAbsMatrixPipe.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpInteractiveClipSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpInteractiveClipSurface *clip = new mafOpInteractiveClipSurface();
  mafDEL(clip);
}
//-----------------------------------------------------------
void mafOpInteractiveClipSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->Update();
  vtkAppendPolyData *append = vtkAppendPolyData::New();
  append->SetInputConnection(sphere->GetOutputPort());
  append->Update();

  mafVMESurface *surfaceInput;
  mafNEW(surfaceInput);
  surfaceInput->SetData(append->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsBefore=append->GetOutput()->GetNumberOfPoints();
  CPPUNIT_ASSERT(numberOfPointsBefore==50);

  mafOpInteractiveClipSurface *clip = new mafOpInteractiveClipSurface();
  clip->TestModeOn();
  clip->SurfaceAccept(surfaceInput);
  clip->SetInput(surfaceInput);
  clip->OpRun();
  int result = clip->Clip();
  CPPUNIT_ASSERT(result==MAF_OK);

  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsAfter=surfaceInput->GetOutput()->GetVTKData()->GetNumberOfPoints();
  CPPUNIT_ASSERT(numberOfPointsAfter==57);

  mafDEL(surfaceInput);

  append->Delete();
  sphere->Delete();

  mafDEL(clip);
}