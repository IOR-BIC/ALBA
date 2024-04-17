/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCleanSurfaceTest
 Authors: Alessandro Chiarini , Matteo Giacomoni
 
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

#include "albaOpCleanSurfaceTest.h"
#include "albaOpCleanSurface.h"

#include "albaString.h"
#include "albaVMESurface.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpCleanSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpCleanSurface *clean=new albaOpCleanSurface();
  albaDEL(clean);
}
//-----------------------------------------------------------
void albaOpCleanSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkSphereSource *sphere1=vtkSphereSource::New();
  sphere1->Update();
  vtkSphereSource *sphere2=vtkSphereSource::New();
  sphere2->Update();
  vtkAppendPolyData *append=vtkAppendPolyData::New();
  append->SetInputConnection(sphere1->GetOutputPort());
  append->AddInputConnection(sphere2->GetOutputPort());
  append->Update();

  albaVMESurface *surfaceInput;
  albaNEW(surfaceInput);
  surfaceInput->SetData(append->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsBefore=append->GetOutput()->GetNumberOfPoints();

  albaOpCleanSurface *clean=new albaOpCleanSurface();
  clean->TestModeOn();
  clean->SetInput(surfaceInput);
  clean->OpRun();
  clean->OnClean();
  clean->OpDo();

  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  int numberOfPointsAfter=surfaceInput->GetOutput()->GetVTKData()->GetNumberOfPoints();

  CPPUNIT_ASSERT(numberOfPointsBefore=2*numberOfPointsAfter);

  albaDEL(surfaceInput);

  append->Delete();
  sphere2->Delete();
  sphere1->Delete();

  albaDEL(clean);
}
