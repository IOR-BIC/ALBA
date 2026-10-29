/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAMeshCutter
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include "vtkALBAMeshCutter.h"
#include "vtkViewport.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLinearTransform.h"
#include "vtkPlane.h"
#include "albaGeometryUtils.h"

vtkStandardNewMacro(vtkALBAMeshCutter);

//----------------------------------------------------------------------------
vtkALBAMeshCutter::vtkALBAMeshCutter()
{
	ExternalPlane = NULL;
	InputTransform = NULL;
	vtkNEW(LocalPlane);
}
//----------------------------------------------------------------------------
vtkALBAMeshCutter::~vtkALBAMeshCutter()
{
	vtkDEL(LocalPlane);
}

//----------------------------------------------------------------------------
void vtkALBAMeshCutter::SetPlane(vtkPlane *plane)
{
	vtk3DLinearGridPlaneCutter::SetPlane(plane);
	ExternalPlane =  plane;
}

//----------------------------------------------------------------------------
int vtkALBAMeshCutter::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (InputTransform && vtkPlane::SafeDownCast(ExternalPlane))
	{
		//Setting a temp cut function to manage InputTranform
		albaGeometryUtils::PlaneToInputSpace((vtkPlane * )ExternalPlane, InputTransform, LocalPlane);
		Plane = LocalPlane;
	}
	else
	{
		Plane = ExternalPlane;
	}

	vtk3DLinearGridPlaneCutter::RequestData( NULL,  inputVector, outputVector);

	Plane = ExternalPlane;

	return 1;
}
