/*=========================================================================

 Program: MAF2Medical
 Module: mafOpSubdivideTest
 Authors: Daniele Giunchi
 
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

#include "mafOpSubdivideTest.h"
#include "mafOpSubdivide.h"

#include "mafString.h"
#include "mafVMESurface.h"
#include "vtkPolyData.h"
#include "vtkSphereSource.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPoints.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpSubdivideTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	mafOpSubdivide *sm=new mafOpSubdivide();
	mafDEL(sm);
}
//-----------------------------------------------------------
void mafOpSubdivideTest::TestOpRun() 
//-----------------------------------------------------------
{
	vtkPolyData *testPolyData;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  testPolyData = sphere->GetOutput();
  testPolyData->Update();

  mafVMESurface *vmeSurface;
  mafNEW(vmeSurface);
  vmeSurface->SetData(testPolyData, 0);
	vmeSurface->GetOutput()->GetVTKData()->Update();
  vmeSurface->Update();
  	
	mafOpSubdivide *subdivideOp=new mafOpSubdivide("Subdivide");
	subdivideOp->TestModeOn();

	CPPUNIT_ASSERT(subdivideOp->Accept(vmeSurface));
  
	subdivideOp->SetInput(vmeSurface);
  subdivideOp->OpRun();
  subdivideOp->SetNumberOfSubdivision(2);
  subdivideOp->OnEvent(&mafEvent(this, mafOpSubdivide::ID_SUBDIVIDE));
	subdivideOp->OpDo();
	
	vmeSurface->GetOutput()->GetVTKData()->Update();
	vmeSurface->Update();

	vtkPoints *pointsSubdivide = vtkPolyData::SafeDownCast(mafVMESurface::SafeDownCast(subdivideOp->GetInput())->GetOutput()->GetVTKData())->GetPoints();
  CPPUNIT_ASSERT(testPolyData->GetNumberOfPoints() <= pointsSubdivide->GetNumberOfPoints() );

  mafDEL(vmeSurface);
	mafDEL(subdivideOp);
  
}