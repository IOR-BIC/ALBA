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

vtkCxxRevisionMacro(vtkALBAFixedCutter, "$Revision: 1.1.2.1 $");
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
void vtkALBAFixedCutter::Execute()
//----------------------------------------------------------------------------
{
  vtkCutter::Execute();
  
  if(this->GetOutput()->GetNumberOfPoints() == 0)
  {
	  vtkPoints *pts = vtkPoints::New();
	  pts->InsertNextPoint(this->GetInput()->GetCenter());
    this->GetOutput()->SetPoints(pts);
    pts->Delete();
  }
  this->GetOutput()->GetPointData()->SetNormals(NULL);
}
