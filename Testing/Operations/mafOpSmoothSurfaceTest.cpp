/*=========================================================================

 Program: MAF2
 Module: mafOpSmoothSurfaceTest
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

#include "mafOpSmoothSurfaceTest.h"
#include "mafOpSmoothSurface.h"

#include "mafString.h"
#include "mafVMESurface.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpSmoothSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpSmoothSurface *Smooth=new mafOpSmoothSurface();
  mafDEL(Smooth);
}
//-----------------------------------------------------------
void mafOpSmoothSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkSphereSource *sphere=vtkSphereSource::New();
  sphere->Update();

  mafVMESurface *surfaceInput;
  mafNEW(surfaceInput);
  surfaceInput->SetData(sphere->GetOutput(),0.0);
  surfaceInput->GetOutput()->GetVTKData()->Update();
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  mafOpSmoothSurface *Smooth=new mafOpSmoothSurface();
  Smooth->TestModeOn();
  Smooth->SetInput(surfaceInput);
  Smooth->OpRun();
  Smooth->OnSmooth();
  Smooth->OpDo();

  mafDEL(surfaceInput);

  sphere->Delete();

  mafDEL(Smooth);
}
