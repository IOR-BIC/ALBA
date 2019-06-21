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
#include "vtkALBAFixTopology.h"
#include "vtkALBAPoissonSurfaceReconstruction.h"

#include "float.h"
#include "vtkObjectFactory.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataNormals.h"

vtkCxxRevisionMacro(vtkALBAFixTopology, "$Revision: 1.1.2.1 $");
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
void vtkALBAFixTopology::Execute()
//----------------------------------------------------------------------------
{
  vtkTriangleFilter *triangle_mesh = vtkTriangleFilter::New();
  triangle_mesh->SetInput(this->GetInput());

  vtkPolyDataNormals *points_with_normal = vtkPolyDataNormals::New();
  points_with_normal->SetInput(triangle_mesh->GetOutput());

  vtkALBAPoissonSurfaceReconstruction *psr_polydata =vtkALBAPoissonSurfaceReconstruction::New();
  psr_polydata->SetInput(points_with_normal->GetOutput());

  psr_polydata->GetOutput()->Update();
  this->GetOutput()->DeepCopy(psr_polydata->GetOutput());  

  //points_with_normal->GetOutput()->Update();
  //this->GetOutput()->DeepCopy(points_with_normal->GetOutput());  

  psr_polydata->Delete();
  points_with_normal->Delete();
  triangle_mesh->Delete();

}