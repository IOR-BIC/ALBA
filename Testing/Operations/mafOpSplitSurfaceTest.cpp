/*=========================================================================

 Program: MAF2Medical
 Module: mafOpSplitSurfaceTest
 Authors: Roberto Mucci
 
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

#include "mafOpSplitSurfaceTest.h"
#include "mafOpSplitSurface.h"

#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"


#include "vtkDataSet.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpSplitSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpSplitSurface *split = new mafOpSplitSurface();
  mafDEL(split);
}
//-----------------------------------------------------------
void mafOpSplitSurfaceTest::TestClip() 
//-----------------------------------------------------------
{
  mafString clippedName = "clipped";
  mafVMESurface *parentSurface;
  mafNEW(parentSurface);

  //Create surface to be  clipped
  mafVMESurfaceParametric *surfaceParametric;
	mafNEW(surfaceParametric);
	surfaceParametric->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_SPHERE);
	surfaceParametric->SetSphereRadius(5.);
	surfaceParametric->Update();
	surfaceParametric->GetOutput()->GetVTKData()->Update();
 
  mafVMESurface *surface;
  mafNEW(surface);
  surface->SetData((vtkPolyData*)surfaceParametric->GetOutput()->GetVTKData(),0.0);
  surface->ReparentTo(parentSurface);

  //Create surface to clip
  mafVMESurfaceParametric *surfaceParametricClip;
  mafNEW(surfaceParametricClip);
  surfaceParametricClip->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_PLANE);
  surfaceParametricClip->Update();
  surfaceParametricClip->GetOutput()->GetVTKData()->Update();

  mafOpSplitSurface *split=new mafOpSplitSurface();
  split->TestModeOn();
  split->SetClippingModality(mafOpSplitSurface::MODE_SURFACE);
  split->SetInput(surface);
  split->SetClippingSurface((mafVMESurface *)surfaceParametricClip);
  split->OpRun();
  split->Clip();
  split->OpDo();

  surface->Update();
  int numberOfChildren = parentSurface->GetNumberOfChildren();
  int numPoints = ((mafVMESurface *)parentSurface->GetLastChild())->GetOutput()->GetVTKData()->GetNumberOfPoints();

  CPPUNIT_ASSERT(numPoints == 61);
  CPPUNIT_ASSERT(clippedName.Compare(parentSurface->GetLastChild()->GetName()) == 0);

  mafDEL(surfaceParametricClip);
	mafDEL(surfaceParametric);
  mafDEL(split);
  mafDEL(surface);
  parentSurface->RemoveAllChildren();
  mafDEL(parentSurface);
}
