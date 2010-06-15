/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDFixTopology.cxx,v $
Language:  C++
Date:      $Date: 2010-06-15 13:56:33 $
Version:   $Revision: 1.1.2.1 $
Authors:   Fuli Wu
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
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