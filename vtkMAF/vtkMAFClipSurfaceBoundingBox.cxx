/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFClipSurfaceBoundingBox.cxx,v $
Language:  C++
Date:      $Date: 2008-07-03 11:27:45 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "vtkObjectFactory.h"

#include "vtkMAFClipSurfaceBoundingBox.h"
#include "vtkMath.h"
#include "vtkClipPolyData.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkMAFImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkClipPolyData.h"

vtkCxxRevisionMacro(vtkMAFClipSurfaceBoundingBox, "$Revision: 1.1 $");  
vtkStandardNewMacro(vtkMAFClipSurfaceBoundingBox);

//-------------------------------------------------------------------------
vtkMAFClipSurfaceBoundingBox::vtkMAFClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
	ClipInside = 0;
}
//-------------------------------------------------------------------------
vtkMAFClipSurfaceBoundingBox::~vtkMAFClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
	SetMask(NULL);
}
//-------------------------------------------------------------------------
void vtkMAFClipSurfaceBoundingBox::Execute() 
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

	vtkMAFImplicitPolyData *implicitPolyData = vtkMAFImplicitPolyData::New();
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
