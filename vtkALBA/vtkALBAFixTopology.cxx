/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixTopology
 Authors: Fuli Wu
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "windows.h"
#include "vtkALBAFixTopology.h"
#include "vtkALBAPoissonSurfaceReconstruction.h"


#include "vtkObjectFactory.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkStandardNewMacro(vtkALBAFixTopology);

//----------------------------------------------------------------------------
vtkALBAFixTopology::vtkALBAFixTopology()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
vtkALBAFixTopology::~vtkALBAFixTopology()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void vtkALBAFixTopology::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkALBAFixTopology::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//----------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));


  vtkTriangleFilter *triangle_mesh = vtkTriangleFilter::New();
  triangle_mesh->SetInputData(input);

  vtkPolyDataNormals *points_with_normal = vtkPolyDataNormals::New();
  points_with_normal->SetInputConnection(triangle_mesh->GetOutputPort());

  vtkALBAPoissonSurfaceReconstruction *psr_polydata =vtkALBAPoissonSurfaceReconstruction::New();
  psr_polydata->SetInputConnection(points_with_normal->GetOutputPort());

  psr_polydata->Update();
  output->DeepCopy(psr_polydata->GetOutput());  

  //points_with_normal->GetOutput()->Update();
  //this->GetOutput()->DeepCopy(points_with_normal->GetOutput());  

  psr_polydata->Delete();
  points_with_normal->Delete();
  triangle_mesh->Delete();

	return 1;
}
