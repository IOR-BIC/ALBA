/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixedCutter
 Authors: Silvano Imboden, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include "vtkALBAFixedCutter.h"
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

vtkStandardNewMacro(vtkALBAFixedCutter);

//----------------------------------------------------------------------------
vtkALBAFixedCutter::vtkALBAFixedCutter()
//----------------------------------------------------------------------------
{
	ExternalCutFunc = NULL;
	InputTransform = NULL;
	vtkNEW(LocalCutFunction);
}
//----------------------------------------------------------------------------
vtkALBAFixedCutter::~vtkALBAFixedCutter()
//----------------------------------------------------------------------------
{
	vtkDEL(LocalCutFunction);
}

//----------------------------------------------------------------------------
vtkMTimeType vtkALBAFixedCutter::GetMTime()
{
	vtkMTimeType mTime = Superclass::GetMTime();
	vtkMTimeType time;

	if (InputTransform != NULL)
	{
		time = InputTransform->GetMTime();
		mTime = (time > mTime ? time : mTime);
	}

	return mTime;
}

//----------------------------------------------------------------------------
void vtkALBAFixedCutter::SetCutFunction(vtkImplicitFunction *cutFunc)
{
	vtkCutter::SetCutFunction(cutFunc);
	ExternalCutFunc =  cutFunc;
}

//----------------------------------------------------------------------------
int vtkALBAFixedCutter::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (InputTransform && vtkPlane::SafeDownCast(ExternalCutFunc))
	{
		//Setting a temp cut function to manage InputTranform
		albaGeometryUtils::PlaneToInputSpace((vtkPlane * )ExternalCutFunc, InputTransform, LocalCutFunction);
		CutFunction = LocalCutFunction;
	}
	else
	{
		CutFunction = ExternalCutFunc;
	}

  vtkCutter::RequestData( NULL,  inputVector, outputVector);

	output->GetPointData()->SetNormals(NULL);

	CutFunction = ExternalCutFunc;
	  
	return 1;
}
