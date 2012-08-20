/*=========================================================================

 Program: MAF2Medical
 Module: vtkMEDFixTopology
 Authors: Fuli Wu
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "vtkMEDFixTopology.h"
#include "vtkMEDPoissonSurfaceReconstruction.h"

#include "float.h"
#include "vtkObjectFactory.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataNormals.h"

vtkCxxRevisionMacro(vtkMEDFixTopology, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMEDFixTopology);

//----------------------------------------------------------------------------
vtkMEDFixTopology::vtkMEDFixTopology()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
vtkMEDFixTopology::~vtkMEDFixTopology()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void vtkMEDFixTopology::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkMEDFixTopology::Execute()
//----------------------------------------------------------------------------
{
  vtkTriangleFilter *triangle_mesh = vtkTriangleFilter::New();
  triangle_mesh->SetInput(this->GetInput());

  vtkPolyDataNormals *points_with_normal = vtkPolyDataNormals::New();
  points_with_normal->SetInput(triangle_mesh->GetOutput());

  vtkMEDPoissonSurfaceReconstruction *psr_polydata =vtkMEDPoissonSurfaceReconstruction::New();
  psr_polydata->SetInput(points_with_normal->GetOutput());

  psr_polydata->GetOutput()->Update();
  this->GetOutput()->DeepCopy(psr_polydata->GetOutput());  

  //points_with_normal->GetOutput()->Update();
  //this->GetOutput()->DeepCopy(points_with_normal->GetOutput());  

  psr_polydata->Delete();
  points_with_normal->Delete();
  triangle_mesh->Delete();

}