/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSmoothSurfaceTest
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

#include "albaOpSmoothSurfaceTest.h"
#include "albaOpSmoothSurface.h"

#include "albaString.h"
#include "albaVMESurface.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpSmoothSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpSmoothSurface *Smooth=new albaOpSmoothSurface();
  albaDEL(Smooth);
}
//-----------------------------------------------------------
void albaOpSmoothSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkSphereSource *sphere=vtkSphereSource::New();
  sphere->Update();

  albaVMESurface *surfaceInput;
  albaNEW(surfaceInput);
  surfaceInput->SetData(sphere->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  albaOpSmoothSurface *Smooth=new albaOpSmoothSurface();
  Smooth->TestModeOn();
  Smooth->SetInput(surfaceInput);
  Smooth->OpRun();
  Smooth->OnSmooth();
  Smooth->OpDo();

  albaDEL(surfaceInput);

  sphere->Delete();

  albaDEL(Smooth);
}
