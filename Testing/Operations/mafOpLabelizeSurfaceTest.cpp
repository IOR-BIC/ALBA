/*=========================================================================

 Program: MAF2
 Module: mafOpLabelizeSurfaceTest
 Authors: Matteo Giacomoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafOpLabelizeSurfaceTest.h"

#include "mafOpLabelizeSurface.h"
#include "mafVMESurface.h"
#include "mafString.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataReader.h"
#include "vtkLookupTable.h"
#include "vtkDoubleArray.h"
#include "vtkCellData.h"
#include "vtkCellCenters.h"

//----------------------------------------------------------------------------
void mafOpLabelizeSurfaceTest::Test()
//----------------------------------------------------------------------------
{
	mafString filename=MAF_DATA_ROOT;
	filename<<"/Surface/Sphere.vtk";
	vtkMAFSmartPointer<vtkPolyDataReader> reader;
	reader->SetFileName(filename.GetCStr());
	reader->Update();

	double boundsInput[6];
	reader->GetOutput()->GetBounds(boundsInput);
	double planeWidth = (boundsInput[1] - boundsInput[0])*2;
	double planeHeight = (boundsInput[3] - boundsInput[2])*2;
	double centerInput[3];
	reader->GetOutput()->GetCenter(centerInput);

	mafSmartPointer<mafVMESurface>surface;
	surface->SetData(reader->GetOutput(),0.0);
	surface->GetOutput()->GetVTKData()->Update();
	surface->Update();
	CPPUNIT_ASSERT(surface);

	vtkLookupTable *lut=vtkLookupTable::New();
	lut->SetNumberOfTableValues(2);
	lut->SetTableValue(0,1.0,0.0,0.0);
	lut->SetTableValue(0,0.0,1.0,0.0);
	lut->Build();

	mafOpLabelizeSurface *labelize=new mafOpLabelizeSurface("Labelize");
	labelize->TestModeOn();
	labelize->SetInput(surface);
	labelize->OpRun();
	labelize->SetPlaneDimension(planeWidth,planeHeight);
	labelize->SetLutEditor(lut);
	labelize->SetLabelValue(1.0);
	labelize->Labelize();
	labelize->OpStop(OP_RUN_OK);
	labelize->OpDo();

	surface->GetOutput()->GetVTKData()->Update();
	surface->Update();

	vtkPolyData *surfaceOutput = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
	vtkDoubleArray *scalars=vtkDoubleArray::SafeDownCast(surfaceOutput->GetCellData()->GetScalars());
	
	double range[2];
	scalars->GetRange(range);
	CPPUNIT_ASSERT(range[0]==0 && range[1]==1);

	vtkMAFSmartPointer<vtkCellCenters> centers;
	centers->SetInput(surfaceOutput);
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
	mafDEL(labelize);
}