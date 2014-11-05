/*=========================================================================

 Program: MAF2Medical
 Module: medOpCreateSurfaceTest
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

#include "medOpCreateSurfaceTest.h"
#include "medOpCreateSurface.h"

#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"


#include "vtkDataSet.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpCreateSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpCreateSurface *create=new medOpCreateSurface();
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpCreateSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  mafVMESurfaceParametric *surfaceParametric;
	mafNEW(surfaceParametric);
	surfaceParametric->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_SPHERE);
	surfaceParametric->SetSphereRadius(5.);
	surfaceParametric->Update();
	surfaceParametric->GetOutput()->GetVTKData()->Update();

  mafVMESurface *surfaceOutput;
  int numberOfPointsBefore=surfaceParametric->GetOutput()->GetVTKData()->GetNumberOfPoints();

  medOpCreateSurface *create=new medOpCreateSurface();
  create->TestModeOn();
  create->SetInput(surfaceParametric);
  create->OpRun();


	surfaceOutput = mafVMESurface::SafeDownCast(create->GetOutput());
  surfaceOutput->GetOutput()->GetVTKData()->Update();
  surfaceOutput->GetOutput()->Update();
  surfaceOutput->Update();

  int numberOfPointsAfter=surfaceOutput->GetOutput()->GetVTKData()->GetNumberOfPoints();

  CPPUNIT_ASSERT(numberOfPointsBefore==numberOfPointsAfter);

	mafDEL(surfaceParametric);
  mafDEL(create);
}
