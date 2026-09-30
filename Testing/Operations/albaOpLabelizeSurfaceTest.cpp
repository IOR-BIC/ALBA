/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpLabelizeSurfaceTest
 Authors: Matteo Giacomoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaOpLabelizeSurfaceTest.h"

#include "albaOpLabelizeSurface.h"
#include "albaVMESurface.h"
#include "albaString.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataReader.h"
#include "vtkLookupTable.h"
#include "vtkDoubleArray.h"
#include "vtkCellData.h"
#include "vtkCellCenters.h"

//----------------------------------------------------------------------------
void albaOpLabelizeSurfaceTest::Test()
//----------------------------------------------------------------------------
{
	albaString filename=ALBA_DATA_ROOT;
	filename<<"/Surface/Sphere.vtk";
	vtkALBASmartPointer<vtkPolyDataReader> reader;
	reader->SetFileName(filename.GetCStr());
	reader->Update();

	double boundsInput[6];
	reader->GetOutput()->GetBounds(boundsInput);
	double planeWidth = (boundsInput[1] - boundsInput[0])*2;
	double planeHeight = (boundsInput[3] - boundsInput[2])*2;
	double centerInput[3];
	reader->GetOutput()->GetCenter(centerInput);

	albaSmartPointer<albaVMESurface>surface;
	surface->SetData(reader->GetOutput(),0.0);
	surface->Update();
	CPPUNIT_ASSERT(surface);

	vtkLookupTable *lut=vtkLookupTable::New();
	lut->SetNumberOfTableValues(2);
	lut->SetTableValue(0,1.0,0.0,0.0);
	lut->SetTableValue(0,0.0,1.0,0.0);
	lut->Build();

	albaOpLabelizeSurface *labelize=new albaOpLabelizeSurface("Labelize");
	labelize->TestModeOn();
	labelize->SetInput(surface);
	labelize->OpRun();
	labelize->SetPlaneDimension(planeWidth,planeHeight);
	labelize->SetLutEditor(lut);
	labelize->SetLabelValue(1.0);
	labelize->Labelize();
	labelize->OpStop(OP_RUN_OK);
	labelize->OpDo();

	surface->Update();

	vtkPolyData *surfaceOutput = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
	vtkDoubleArray *scalars=vtkDoubleArray::SafeDownCast(surfaceOutput->GetCellData()->GetScalars());
	
	double range[2];
	scalars->GetRange(range);
	CPPUNIT_ASSERT(range[0]==0 && range[1]==1);

	vtkALBASmartPointer<vtkCellCenters> centers;
	centers->SetInputData(surfaceOutput);
	centers->Update();

	for(int i=0;i<centers->GetOutput()->GetNumberOfPoints();i++)
	{
		double centerCell[3];
		centers->GetOutput()->GetPoint(i,centerCell);
		if(centerInput[2]<centerCell[2])
		{
			CPPUNIT_ASSERT(scalars->GetValue(i)==1.0);
		}
		else
		{
			CPPUNIT_ASSERT(scalars->GetValue(i)==0.0);
		}
	}

	lut->Delete();
	albaDEL(labelize);
}