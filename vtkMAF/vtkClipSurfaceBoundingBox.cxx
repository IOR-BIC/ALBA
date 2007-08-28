/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkClipSurfaceBoundingBox.cxx,v $
Language:  C++
Date:      $Date: 2007-08-28 11:27:29 $
Version:   $Revision: 1.4 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "vtkObjectFactory.h"

#include "vtkClipSurfaceBoundingBox.h"
#include "vtkMath.h"
#include "vtkClipPolyData.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkClipPolyData.h"

vtkCxxRevisionMacro(vtkClipSurfaceBoundingBox, "$Revision: 1.4 $");  
vtkStandardNewMacro(vtkClipSurfaceBoundingBox);

//-------------------------------------------------------------------------
vtkClipSurfaceBoundingBox::vtkClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
	ClipInside = 0;
}
//-------------------------------------------------------------------------
vtkClipSurfaceBoundingBox::~vtkClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
	SetMask(NULL);
}
//-------------------------------------------------------------------------
void vtkClipSurfaceBoundingBox::Execute() 
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

	vtkImplicitPolyData *implicitPolyData = vtkImplicitPolyData::New();
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
