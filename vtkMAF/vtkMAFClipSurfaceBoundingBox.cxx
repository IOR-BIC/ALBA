/*=========================================================================

 Program: MAF2
 Module: vtkMAFClipSurfaceBoundingBox
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkObjectFactory.h"

#include "vtkMAFClipSurfaceBoundingBox.h"
#include "vtkMath.h"
#include "vtkClipPolyData.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkMAFImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkClipPolyData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkStandardNewMacro(vtkMAFClipSurfaceBoundingBox);

//-------------------------------------------------------------------------
vtkMAFClipSurfaceBoundingBox::vtkMAFClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
	ClipInside = 0;
	SetNumberOfInputPorts(2);
}
//-------------------------------------------------------------------------
vtkMAFClipSurfaceBoundingBox::~vtkMAFClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
	
}
//-------------------------------------------------------------------------
int vtkMAFClipSurfaceBoundingBox::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//-------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	vtkPolyData *mask		=	this->GetMask();
	
	double bounds[6];
	input->GetBounds(bounds);
	double p1[3],p2[3];
	for(int i=0;i<3;i++)
	{
		p1[i]=bounds[i*2];
		p2[i]=bounds[i*2+1];
	}
	double scale_factor=2*sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

	vtkLinearExtrusionFilter *extrusionFilter = vtkLinearExtrusionFilter::New();
	extrusionFilter->SetInputData(mask);
	extrusionFilter->SetScaleFactor(scale_factor);
	extrusionFilter->Modified();
	extrusionFilter->Update();

	vtkMAFImplicitPolyData *implicitPolyData = vtkMAFImplicitPolyData::New();
	implicitPolyData->SetInput(extrusionFilter->GetOutput());

	vtkClipPolyData *clipFilter = vtkClipPolyData::New();
	clipFilter->SetInputData(input);
	clipFilter->SetGenerateClipScalars(0);
	clipFilter->SetClipFunction(implicitPolyData);
	clipFilter->SetInsideOut(ClipInside);
	clipFilter->SetValue(0);
	clipFilter->Update();

	output->DeepCopy(clipFilter->GetOutput());

	clipFilter->Delete();
	implicitPolyData->Delete();
	extrusionFilter->Delete();

	return 1;
}
