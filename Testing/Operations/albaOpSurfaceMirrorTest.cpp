/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSurfaceMirrorTest
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

#include "albaOpSurfaceMirrorTest.h"
#include "albaOpSurfaceMirror.h"

#include "albaString.h"
#include "albaVMESurface.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpSurfaceMirrorTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	albaOpSurfaceMirror *sm=new albaOpSurfaceMirror();
	albaDEL(sm);
}
//-----------------------------------------------------------
void albaOpSurfaceMirrorTest::TestOpRun() 
//-----------------------------------------------------------
{
	vtkPolyData *testPolyData = vtkPolyData::New();

	vtkPoints   *points = vtkPoints::New();
	vtkCellArray   *cellArray = vtkCellArray::New();
	testPolyData->SetPoints(points);
	testPolyData->SetPolys(cellArray);

	double zero = 0.0;
	vtkIdType pointId[3];
	points->InsertNextPoint(zero,zero,zero);
	points->InsertNextPoint(0.5,0.5,0.5);
	points->InsertNextPoint(1.0,1.0,3.0);

	pointId[0] = 0;
	pointId[1] = 1;
	pointId[2] = 2;
	cellArray->InsertNextCell(3 , pointId);

  albaVMESurface *vmeSurface;
  albaNEW(vmeSurface);
  vmeSurface->SetData(testPolyData, 0);
  vmeSurface->Update();
  	
	albaOpSurfaceMirror *surfaceMirrorOp=new albaOpSurfaceMirror("Surface Mirror");
	surfaceMirrorOp->TestModeOn();

	CPPUNIT_ASSERT(surfaceMirrorOp->Accept(vmeSurface));
  
	surfaceMirrorOp->SetInput(vmeSurface);
  surfaceMirrorOp->OpRun();
	surfaceMirrorOp->OpDo();
	
	vmeSurface->Update();

	
	vtkPoints *pointsMirror = vtkPolyData::SafeDownCast(albaVMESurface::SafeDownCast(surfaceMirrorOp->GetInput())->GetOutput()->GetVTKData())->GetPoints();
  for(int i=0; i<points->GetNumberOfPoints();i++)
	{
		double value = points->GetPoint(i)[0] ;
		double valueMirror = - pointsMirror->GetPoint(i)[0];
		//printf("%f %f \n", value, valueMirror);
    CPPUNIT_ASSERT(value == valueMirror );
	}
	
	
  albaDEL(vmeSurface);
	vtkDEL(testPolyData);
  vtkDEL(cellArray);
	vtkDEL(points);
	albaDEL(surfaceMirrorOp);
  
}