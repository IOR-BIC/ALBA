/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAMeshLinearizationFilter
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkALBAMeshLinearizationFilter.h"

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellType.h>
#include <vtkFieldData.h>
#include <vtkIdList.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>


#include <iostream>
#include <string>
#include "albaDefines.h"
#include "albaString.h"



vtkStandardNewMacro(vtkALBAMeshLinearizationFilter);

//------------------------------------------------------------------------------
vtkALBAMeshLinearizationFilter::vtkALBAMeshLinearizationFilter()
	: RemoveUnusedPoints(true)
{
	this->SetNumberOfInputPorts(1);
	this->SetNumberOfOutputPorts(1);
}
//------------------------------------------------------------------------------
int vtkALBAMeshLinearizationFilter::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	vtkUnstructuredGrid *input = vtkUnstructuredGrid::GetData(inputVector[0], 0);
	vtkUnstructuredGrid *output = vtkUnstructuredGrid::GetData(outputVector, 0);

	if (input == nullptr || output == nullptr)
	{
		vtkErrorMacro("Missing input or output unstructured grid.");
		return 0;
	}

	int m_time = input->GetMTime();

	const vtkIdType numCells = input->GetNumberOfCells();
	const vtkIdType numInputPoints = input->GetNumberOfPoints();
	vtkPoints *inputPoints = input->GetPoints();

	if (numCells == 0 || inputPoints == nullptr)
	{
		output->ShallowCopy(input);
		return 1;
	}

	for (vtkIdType cellId = 0; cellId < numCells; ++cellId)
	{
		if (input->GetCellType(cellId) == VTK_POLYHEDRON)
		{
			vtkErrorMacro("Polyhedral cells are not supported.");
			return 0;
		}
	}

	std::vector<vtkIdType> pointMap;
	if (this->RemoveUnusedPoints)
	{
		pointMap.assign(static_cast<size_t>(numInputPoints), -1);
	}
	vtkIdType numOutputPoints = 0;

	output->AllocateExact(numCells, input->GetCells()->GetNumberOfConnectivityIds());

	vtkNew<vtkIdList> inputCellPoints;
	vtkNew<vtkIdList> outputCellPoints;

	for (vtkIdType cellId = 0; cellId < numCells; ++cellId)
	{
		const int inputType = input->GetCellType(cellId);
		input->GetCellPoints(cellId, inputCellPoints);

		int outputType = inputType;
		vtkIdType numKeptPoints = inputCellPoints->GetNumberOfIds();

		int linearType = 0;
		vtkIdType numCornerPoints = 0;
		if (GetLinearCellInfo(inputType, linearType, numCornerPoints) &&
			numCornerPoints <= numKeptPoints)
		{
			outputType = linearType;
			numKeptPoints = numCornerPoints;
		}

		outputCellPoints->SetNumberOfIds(numKeptPoints);
		for (vtkIdType i = 0; i < numKeptPoints; ++i)
		{
			vtkIdType pointId = inputCellPoints->GetId(i);

			if (this->RemoveUnusedPoints)
			{
				vtkIdType &mapped = pointMap[static_cast<size_t>(pointId)];
				if (mapped < 0)
				{
					mapped = numOutputPoints++;
				}
				pointId = mapped;
			}

			outputCellPoints->SetId(i, pointId);
		}

		output->InsertNextCell(outputType, outputCellPoints);
	}

	if (this->RemoveUnusedPoints)
	{
		vtkNew<vtkPoints> outputPoints;
		outputPoints->SetDataType(inputPoints->GetDataType());
		outputPoints->SetNumberOfPoints(numOutputPoints);

		vtkPointData *inputPointData = input->GetPointData();
		vtkPointData *outputPointData = output->GetPointData();
		outputPointData->CopyAllocate(inputPointData, numOutputPoints);

		for (vtkIdType oldId = 0; oldId < numInputPoints; ++oldId)
		{
			const vtkIdType newId = pointMap[static_cast<size_t>(oldId)];
			if (newId >= 0)
			{
				outputPoints->SetPoint(newId, inputPoints->GetPoint(oldId));
				outputPointData->CopyData(inputPointData, oldId, newId);
			}
		}

		output->SetPoints(outputPoints);
	}
	else
	{
		output->SetPoints(inputPoints);
		output->GetPointData()->ShallowCopy(input->GetPointData());
	}

	output->GetCellData()->ShallowCopy(input->GetCellData());
	output->GetFieldData()->ShallowCopy(input->GetFieldData());
	output->Squeeze();

	return 1;
}

//----------------------------------------------------------------------------
bool vtkALBAMeshLinearizationFilter::GetLinearCellInfo(int cellType, int &linearType, vtkIdType &numCornerPoints)
{
	switch (cellType)
	{
	case VTK_QUADRATIC_EDGE:
	case VTK_CUBIC_LINE:
	case VTK_LAGRANGE_CURVE:
	case VTK_BEZIER_CURVE:
		linearType = VTK_LINE;
		numCornerPoints = 2;
		return true;

	case VTK_QUADRATIC_TRIANGLE:
	case VTK_BIQUADRATIC_TRIANGLE:
	case VTK_LAGRANGE_TRIANGLE:
	case VTK_BEZIER_TRIANGLE:
		linearType = VTK_TRIANGLE;
		numCornerPoints = 3;
		return true;

	case VTK_QUADRATIC_QUAD:
	case VTK_BIQUADRATIC_QUAD:
	case VTK_QUADRATIC_LINEAR_QUAD:
	case VTK_LAGRANGE_QUADRILATERAL:
	case VTK_BEZIER_QUADRILATERAL:
		linearType = VTK_QUAD;
		numCornerPoints = 4;
		return true;

	case VTK_QUADRATIC_TETRA:
	case VTK_LAGRANGE_TETRAHEDRON:
	case VTK_BEZIER_TETRAHEDRON:
		linearType = VTK_TETRA;
		numCornerPoints = 4;
		return true;

	case VTK_QUADRATIC_HEXAHEDRON:
	case VTK_TRIQUADRATIC_HEXAHEDRON:
	case VTK_LAGRANGE_HEXAHEDRON:
	case VTK_BEZIER_HEXAHEDRON:
		linearType = VTK_HEXAHEDRON;
		numCornerPoints = 8;
		return true;

	case VTK_QUADRATIC_WEDGE:
	case VTK_QUADRATIC_LINEAR_WEDGE:
	case VTK_BIQUADRATIC_QUADRATIC_WEDGE:
	case VTK_LAGRANGE_WEDGE:
	case VTK_BEZIER_WEDGE:
		linearType = VTK_WEDGE;
		numCornerPoints = 6;
		return true;

	case VTK_QUADRATIC_PYRAMID:
	case VTK_TRIQUADRATIC_PYRAMID:
		linearType = VTK_PYRAMID;
		numCornerPoints = 5;
		return true;

	default:
		return false;
	}
}

//------------------------------------------------------------------------------
void vtkALBAMeshLinearizationFilter::PrintSelf(ostream &os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	os << indent << "RemoveUnusedPoints: " << (this->RemoveUnusedPoints ? "On" : "Off") << "\n";
}