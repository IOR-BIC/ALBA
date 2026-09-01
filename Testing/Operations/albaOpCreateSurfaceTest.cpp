/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSurfaceTest
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

#include "albaOpCreateSurfaceTest.h"
#include "albaOpCreateSurface.h"

#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"


#include "vtkDataSet.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpCreateSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpCreateSurface *create=new albaOpCreateSurface();
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpCreateSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  albaVMESurfaceParametric *surfaceParametric;
	albaNEW(surfaceParametric);
	surfaceParametric->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_SPHERE);
	surfaceParametric->SetSphereRadius(5.);
	surfaceParametric->Update();

  albaVMESurface *surfaceOutput;
  int numberOfPointsBefore=surfaceParametric->GetOutput()->GetVTKData()->GetNumberOfPoints();

  albaOpCreateSurface *create=new albaOpCreateSurface();
  create->TestModeOn();
  create->SetInput(surfaceParametric);
  create->OpRun();


	surfaceOutput = albaVMESurface::SafeDownCast(create->GetOutput());
  surfaceOutput->GetOutput()->Update();
  surfaceOutput->Update();

  int numberOfPointsAfter=surfaceOutput->GetOutput()->GetVTKData()->GetNumberOfPoints();

  CPPUNIT_ASSERT(numberOfPointsBefore==numberOfPointsAfter);

	albaDEL(surfaceParametric);
  albaDEL(create);
}
