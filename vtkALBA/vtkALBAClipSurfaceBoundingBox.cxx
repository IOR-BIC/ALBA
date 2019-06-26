/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAClipSurfaceBoundingBox
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkObjectFactory.h"

#include "vtkALBAClipSurfaceBoundingBox.h"
#include "vtkMath.h"
#include "vtkClipPolyData.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkALBAImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkClipPolyData.h"

vtkCxxRevisionMacro(vtkALBAClipSurfaceBoundingBox, "$Revision: 1.1 $");  
vtkStandardNewMacro(vtkALBAClipSurfaceBoundingBox);

//-------------------------------------------------------------------------
vtkALBAClipSurfaceBoundingBox::vtkALBAClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
	ClipInside = 0;
}
//-------------------------------------------------------------------------
vtkALBAClipSurfaceBoundingBox::~vtkALBAClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
	SetMask(NULL);
}
//-------------------------------------------------------------------------
void vtkALBAClipSurfaceBoundingBox::Execute() 
//-------------------------------------------------------------------------
{
	vtkPolyData *output = this->GetOutput();
	vtkPolyData *input	= this->GetInput();
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
	extrusionFilter->SetInput(mask);
	extrusionFilter->SetScaleFactor(scale_factor);
	extrusionFilter->Modified();
	extrusionFilter->Update();

	vtkALBAImplicitPolyData *implicitPolyData = vtkALBAImplicitPolyData::New();
	implicitPolyData->SetInput(extrusionFilter->GetOutput());

	vtkClipPolyData *clipFilter = vtkClipPolyData::New();
	clipFilter->SetInput(input);
	clipFilter->SetGenerateClipScalars(0);
	clipFilter->SetClipFunction(implicitPolyData);
	clipFilter->SetInsideOut(ClipInside);
	clipFilter->SetValue(0);
	clipFilter->Update();

	output->DeepCopy(clipFilter->GetOutput());

	clipFilter->Delete();
	implicitPolyData->Delete();
	extrusionFilter->Delete();
}
