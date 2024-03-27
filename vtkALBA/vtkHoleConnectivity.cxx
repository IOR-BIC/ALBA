/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkHoleConnectivity.cxx
Language:  C++
Version:   $Id: vtkHoleConnectivity.cxx,v 1.3.2.2 2011-05-26 08:33:31 ior02 Exp $

Copyright (c) 2003-2004 Goodwin Lawlor
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE. 

Some modifications by Matteo Giacomoni in order to make it work
under ALBA

=========================================================================*/

#include "vtkHoleConnectivity.h"

#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyData.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkStandardNewMacro(vtkHoleConnectivity);

//----------------------------------------------------------------------------
vtkHoleConnectivity::vtkHoleConnectivity(vtkPolyData *input,vtkIdType ID)
//----------------------------------------------------------------------------
{
	this->SetInputData(input);
	PointID = ID;
}
//----------------------------------------------------------------------------
vtkHoleConnectivity::~vtkHoleConnectivity()
//----------------------------------------------------------------------------
{  
}
//----------------------------------------------------------------------------
int vtkHoleConnectivity::RequestData(vtkInformation *request,	vtkInformationVector **inputVector,	vtkInformationVector *outputVector)
//----------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	this->SetExtractionModeToClosestPointRegion();
	this->SetClosestPoint(Point);
		
	return vtkPolyDataConnectivityFilter::RequestData(request,inputVector,outputVector);
}
//----------------------------------------------------------------------------
void vtkHoleConnectivity::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
	vtkPolyDataAlgorithm::PrintSelf(os,indent);
}

