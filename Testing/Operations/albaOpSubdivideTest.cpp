/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSubdivideTest
 Authors: Daniele Giunchi
 
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

#include "albaOpSubdivideTest.h"
#include "albaOpSubdivide.h"

#include "albaString.h"
#include "albaVMESurface.h"
#include "vtkPolyData.h"
#include "vtkSphereSource.h"
#include "vtkALBASmartPointer.h"
#include "vtkPoints.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpSubdivideTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	albaOpSubdivide *sm=new albaOpSubdivide();
	albaDEL(sm);
}
//-----------------------------------------------------------
void albaOpSubdivideTest::TestOpRun() 
//-----------------------------------------------------------
{
	vtkPolyData *testPolyData;
  vtkALBASmartPointer<vtkSphereSource> sphere;
	sphere->Update();

  testPolyData = sphere->GetOutput();

  albaVMESurface *vmeSurface;
  albaNEW(vmeSurface);
  vmeSurface->SetData(testPolyData, 0);
  vmeSurface->Update();
  	
	albaOpSubdivide *subdivideOp=new albaOpSubdivide("Subdivide");
	subdivideOp->TestModeOn();

	CPPUNIT_ASSERT(subdivideOp->Accept(vmeSurface));
  
	subdivideOp->SetInput(vmeSurface);
  subdivideOp->OpRun();
  subdivideOp->SetNumberOfSubdivision(2);
  subdivideOp->OnEvent(&albaEvent(this, albaOpSubdivide::ID_SUBDIVIDE));
	subdivideOp->OpDo();
	
	vmeSurface->Update();

	vtkPoints *pointsSubdivide = vtkPolyData::SafeDownCast(albaVMESurface::SafeDownCast(subdivideOp->GetInput())->GetOutput()->GetVTKData())->GetPoints();
  CPPUNIT_ASSERT(testPolyData->GetNumberOfPoints() <= pointsSubdivide->GetNumberOfPoints() );

  albaDEL(vmeSurface);
	albaDEL(subdivideOp);
  
}