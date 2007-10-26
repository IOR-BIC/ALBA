/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpLabelizeSurfaceTest.cpp,v $
Language:  C++
Date:      $Date: 2007-10-26 13:17:41 $
Version:   $Revision: 1.3 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 

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
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medOpLabelizeSurfaceTest.h"

#include "medOpLabelizeSurface.h"
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
void medOpLabelizeSurfaceTest::Test()
//----------------------------------------------------------------------------
{
	mafString filename=MED_DATA_ROOT;
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

	medOpLabelizeSurface *labelize=new medOpLabelizeSurface("Labelize");
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