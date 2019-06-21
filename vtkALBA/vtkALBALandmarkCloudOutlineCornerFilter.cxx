/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAPointsOutlineCornerFilter.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen, Gianluigi Crimi
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkALBALandmarkCloudOutlineCornerFilter.h"

#include "vtkDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkOutlineCornerSource.h"
#include "vtkPolyData.h"

vtkCxxRevisionMacro(vtkALBALandmarkCloudOutlineCornerFilter, "$Revision: 1.9 $");
vtkStandardNewMacro(vtkALBALandmarkCloudOutlineCornerFilter);

vtkALBALandmarkCloudOutlineCornerFilter::vtkALBALandmarkCloudOutlineCornerFilter ()
{
  this->CornerFactor = 0.2;
	this->CloudRadius = 1.0;
  this->OutlineCornerSource = vtkOutlineCornerSource::New();
}

vtkALBALandmarkCloudOutlineCornerFilter::~vtkALBALandmarkCloudOutlineCornerFilter ()
{
  if (this->OutlineCornerSource != NULL)
    {
    this->OutlineCornerSource->Delete ();
    this->OutlineCornerSource = NULL;
    }
}

void vtkALBALandmarkCloudOutlineCornerFilter::Execute()
{
  vtkPolyData *output = this->GetOutput();
  
  vtkDebugMacro(<< "Creating dataset outline");
	double bounds[6];

  //
  // Let OutlineCornerSource do all the work
  //
	((vtkPointSet *)GetInput())->GetPoints()->GetBounds(bounds);
	for (int i = 0; i < 6; i += 2)
		bounds[i] -= CloudRadius;
	for (int i = 1; i < 6; i += 2)
		bounds[i] += CloudRadius;

	this->OutlineCornerSource->SetBounds(bounds);
  this->OutlineCornerSource->SetCornerFactor(this->GetCornerFactor());
  this->OutlineCornerSource->Update();

  output->CopyStructure(this->OutlineCornerSource->GetOutput());

}


void vtkALBALandmarkCloudOutlineCornerFilter::ExecuteInformation()
{
  vtkDebugMacro(<< "Creating dataset outline");

  //
  // Let OutlineCornerSource do all the work
  //
  
  this->vtkSource::ExecuteInformation();

  this->OutlineCornerSource->UpdateInformation();
}


void vtkALBALandmarkCloudOutlineCornerFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "CornerFactor: " << this->CornerFactor << "\n";
}
