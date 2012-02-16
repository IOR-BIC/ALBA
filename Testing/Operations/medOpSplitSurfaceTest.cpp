/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpSplitSurfaceTest.cpp,v $
Language:  C++
Date:      $Date: 2012-02-16 14:38:00 $
Version:   $Revision: 1.1.2.2 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2007
CINECA - Interuniversity Consortium (www.cineca.it)
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpSplitSurfaceTest.h"
#include "medOpSplitSurface.h"

#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"


#include "vtkDataSet.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpSplitSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpSplitSurface *split = new medOpSplitSurface();
  mafDEL(split);
}
//-----------------------------------------------------------
void medOpSplitSurfaceTest::TestClip() 
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

  medOpSplitSurface *split=new medOpSplitSurface();
  split->TestModeOn();
  split->SetClippingModality(medOpSplitSurface::MODE_SURFACE);
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
