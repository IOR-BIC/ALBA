/*=========================================================================

 Program: MAF2
 Module: mafOpCleanSurfaceTest
 Authors: Alessandro Chiarini , Matteo Giacomoni
 
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

#include "mafOpCleanSurfaceTest.h"
#include "mafOpCleanSurface.h"

#include "mafString.h"
#include "mafVMESurface.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpCleanSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpCleanSurface *clean=new mafOpCleanSurface();
  mafDEL(clean);
}
//-----------------------------------------------------------
void mafOpCleanSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkSphereSource *sphere1=vtkSphereSource::New();
  sphere1->Update();
  vtkSphereSource *sphere2=vtkSphereSource::New();
  sphere2->Update();
  vtkAppendPolyData *append=vtkAppendPolyData::New();
  append->SetInput(sphere1->GetOutput());
  append->AddInput(sphere2->GetOutput());
  append->Update();

  mafVMESurface *surfaceInput;
  mafNEW(surfaceInput);
  surfaceInput->SetData(append->GetOutput(),0.0);
  surfaceInput->GetOutput()->GetVTKData()->Update();
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsBefore=append->GetOutput()->GetNumberOfPoints();

  mafOpCleanSurface *clean=new mafOpCleanSurface();
  clean->TestModeOn();
  clean->SetInput(surfaceInput);
  clean->OpRun();
  clean->OnClean();
  clean->OpDo();

  surfaceInput->GetOutput()->GetVTKData()->Update();
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsAfter=surfaceInput->GetOutput()->GetVTKData()->GetNumberOfPoints();

  CPPUNIT_ASSERT(numberOfPointsBefore=2*numberOfPointsAfter);

  mafDEL(surfaceInput);

  append->Delete();
  sphere2->Delete();
  sphere1->Delete();

  mafDEL(clean);
}
