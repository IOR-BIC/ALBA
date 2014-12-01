/*=========================================================================

 Program: MAF2Medical
 Module: mafOpSurfaceMirrorTest
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

#include "mafOpSurfaceMirrorTest.h"
#include "mafOpSurfaceMirror.h"

#include "mafString.h"
#include "mafVMESurface.h"
#include "vtkPolyData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpSurfaceMirrorTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	mafOpSurfaceMirror *sm=new mafOpSurfaceMirror();
	mafDEL(sm);
}
//-----------------------------------------------------------
void mafOpSurfaceMirrorTest::TestOpRun() 
//-----------------------------------------------------------
{
	vtkPolyData *testPolyData = vtkPolyData::New();

	vtkPoints   *points = vtkPoints::New();
	vtkCellArray   *cellArray = vtkCellArray::New();
	testPolyData->SetPoints(points);
	testPolyData->SetPolys(cellArray);

	double zero = 0.0;
	int pointId[3];
	points->InsertNextPoint(zero,zero,zero);
	points->InsertNextPoint(0.5,0.5,0.5);
	points->InsertNextPoint(1.0,1.0,3.0);

	pointId[0] = 0;
	pointId[1] = 1;
	pointId[2] = 2;
	cellArray->InsertNextCell(3 , pointId);

	testPolyData->Update();

  	
  mafVMESurface *vmeSurface;
  mafNEW(vmeSurface);
  vmeSurface->SetData(testPolyData, 0);
	vmeSurface->GetOutput()->GetVTKData()->Update();
  vmeSurface->Update();
  	
	mafOpSurfaceMirror *surfaceMirrorOp=new mafOpSurfaceMirror("Surface Mirror");
	surfaceMirrorOp->TestModeOn();

	CPPUNIT_ASSERT(surfaceMirrorOp->Accept(vmeSurface));
  
	surfaceMirrorOp->SetInput(vmeSurface);
  surfaceMirrorOp->OpRun();
	surfaceMirrorOp->OpDo();
	
	vmeSurface->GetOutput()->GetVTKData()->Update();
	vmeSurface->Update();

	
	vtkPoints *pointsMirror = vtkPolyData::SafeDownCast(mafVMESurface::SafeDownCast(surfaceMirrorOp->GetInput())->GetOutput()->GetVTKData())->GetPoints();
  for(int i=0; i<points->GetNumberOfPoints();i++)
	{
		double value = points->GetPoint(i)[0] ;
		double valueMirror = - pointsMirror->GetPoint(i)[0];
		//printf("%f %f \n", value, valueMirror);
    CPPUNIT_ASSERT(value == valueMirror );
	}
	
	
  mafDEL(vmeSurface);
	vtkDEL(testPolyData);
  vtkDEL(cellArray);
	vtkDEL(points);
	mafDEL(surfaceMirrorOp);
  
}