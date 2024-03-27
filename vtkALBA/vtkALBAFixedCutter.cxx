/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixedCutter
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAFixedCutter.h"
#include "vtkViewport.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkStandardNewMacro(vtkALBAFixedCutter);

//----------------------------------------------------------------------------
vtkALBAFixedCutter::vtkALBAFixedCutter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
vtkALBAFixedCutter::~vtkALBAFixedCutter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int vtkALBAFixedCutter::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//----------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkCutter::RequestData( NULL,  inputVector, outputVector);
	  
  if(output->GetNumberOfPoints() == 0)
  {
	  vtkPoints *pts = vtkPoints::New();
	  pts->InsertNextPoint(input->GetCenter());
    output->SetPoints(pts);
    pts->Delete();
  }
  output->GetPointData()->SetNormals(NULL);

	return 1;
}
