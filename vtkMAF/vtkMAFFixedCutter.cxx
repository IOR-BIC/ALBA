/*=========================================================================

 Program: MAF2
 Module: vtkMAFFixedCutter
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFFixedCutter.h"
#include "vtkViewport.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"

vtkCxxRevisionMacro(vtkMAFFixedCutter, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFFixedCutter);

//----------------------------------------------------------------------------
vtkMAFFixedCutter::vtkMAFFixedCutter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
vtkMAFFixedCutter::~vtkMAFFixedCutter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFFixedCutter::Execute()
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
