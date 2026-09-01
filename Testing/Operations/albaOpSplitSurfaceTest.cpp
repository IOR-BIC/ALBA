/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSplitSurfaceTest
 Authors: Roberto Mucci
 
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

#include "albaOpSplitSurfaceTest.h"
#include "albaOpSplitSurface.h"

#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"


#include "vtkDataSet.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpSplitSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpSplitSurface *split = new albaOpSplitSurface();
  albaDEL(split);
}
//-----------------------------------------------------------
void albaOpSplitSurfaceTest::TestClip() 
//-----------------------------------------------------------
{
  albaString clippedName = "clipped";
  albaVMESurface *parentSurface;
  albaNEW(parentSurface);

  //Create surface to be  clipped
  albaVMESurfaceParametric *surfaceParametric;
	albaNEW(surfaceParametric);
	surfaceParametric->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_SPHERE);
	surfaceParametric->SetSphereRadius(5.);
	surfaceParametric->Update();
 
  albaVMESurface *surface;
  albaNEW(surface);
  surface->SetData((vtkPolyData*)surfaceParametric->GetOutput()->GetVTKData(),0.0);
  surface->ReparentTo(parentSurface);

  //Create surface to clip
  albaVMESurfaceParametric *surfaceParametricClip;
  albaNEW(surfaceParametricClip);
  surfaceParametricClip->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_PLANE);
  surfaceParametricClip->Update();

  albaOpSplitSurface *split=new albaOpSplitSurface();
  split->TestModeOn();
  split->SetClippingModality(albaOpSplitSurface::MODE_SURFACE);
  split->SetInput(surface);
  split->SetClippingSurface((albaVMESurface *)surfaceParametricClip);
  split->OpRun();
  split->Clip();
  split->OpDo();

  surface->Update();
  int numberOfChildren = parentSurface->GetNumberOfChildren();
  int numPoints = ((albaVMESurface *)parentSurface->GetLastChild())->GetOutput()->GetVTKData()->GetNumberOfPoints();

  CPPUNIT_ASSERT(numPoints == 61);
  CPPUNIT_ASSERT(clippedName.Compare(parentSurface->GetLastChild()->GetName()) == 0);

  albaDEL(surfaceParametricClip);
	albaDEL(surfaceParametric);
  albaDEL(split);
  albaDEL(surface);
  parentSurface->RemoveAllChildren();
  albaDEL(parentSurface);
}
