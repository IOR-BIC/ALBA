/*=========================================================================

Program: ALBA
Module: vtkALBAEllipseSource
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAConnectedRegionsContourTriangulator.h"

#include <vtkAppendPolyData.h>
#include <vtkContourTriangulator.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataConnectivityFilter.h>

vtkStandardNewMacro(vtkALBAConnectedRegionsContourTriangulator);

int vtkALBAConnectedRegionsContourTriangulator::RequestData(
	vtkInformation *,
	vtkInformationVector **inputVector,
	vtkInformationVector *outputVector)
{
	vtkPolyData *input = vtkPolyData::GetData(inputVector[0]);
	vtkPolyData *output = vtkPolyData::GetData(outputVector);

	// Count the number of connected regions in the input
	vtkNew<vtkPolyDataConnectivityFilter> connectivity;
	connectivity->SetInputData(input);
	connectivity->SetExtractionModeToAllRegions();
	connectivity->Update();
	int numRegions = connectivity->GetNumberOfExtractedRegions();

	vtkNew<vtkAppendPolyData> append;

	for (int i = 0; i < numRegions; ++i)
	{
		vtkNew<vtkPolyDataConnectivityFilter> region;
		region->SetInputData(input);
		region->SetExtractionModeToSpecifiedRegions();
		region->InitializeSpecifiedRegionList();
		region->AddSpecifiedRegion(i);
		region->Update();

		vtkNew<vtkContourTriangulator> triangulator;
		triangulator->SetInputConnection(region->GetOutputPort());
		triangulator->Update();

		if (triangulator->GetOutput()->GetNumberOfCells() > 0)
			append->AddInputData(triangulator->GetOutput());
	}

	if (append->GetNumberOfInputConnections(0) > 0)
	{
		append->Update();
		output->ShallowCopy(append->GetOutput());
	}

	return 1;
}
