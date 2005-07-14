/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkFixedCutter.cxx,v $
  Language:  C++
  Date:      $Date: 2005-07-14 07:39:29 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/

#include "vtkFixedCutter.h"
#include "vtkViewport.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"

//vtkCxxRevisionMacro(vtkFixedCutter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkFixedCutter);

//----------------------------------------------------------------------------
vtkFixedCutter::vtkFixedCutter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
vtkFixedCutter::~vtkFixedCutter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkFixedCutter::Execute()
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
