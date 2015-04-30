/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkHoleConnectivity.cxx
Language:  C++
Version:   $Id: vtkHoleConnectivity.cxx,v 1.3.2.2 2011-05-26 08:33:31 ior02 Exp $

Copyright (c) 2003-2004 Goodwin Lawlor
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE. 

Some modifications by Matteo Giacomoni in order to make it work
under MAF (www.openmaf.org)

=========================================================================*/

#include "vtkHoleConnectivity.h"

#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyData.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkHoleConnectivity);

//----------------------------------------------------------------------------
vtkHoleConnectivity::vtkHoleConnectivity(vtkPolyData *input,vtkIdType ID)
//----------------------------------------------------------------------------
{
	this->SetInput(input);
	PointID = ID;
}
//----------------------------------------------------------------------------
vtkHoleConnectivity::~vtkHoleConnectivity()
//----------------------------------------------------------------------------
{  
}
//----------------------------------------------------------------------------
void vtkHoleConnectivity::Execute()
//----------------------------------------------------------------------------
{
	vtkPolyData *output = this->GetOutput();
	vtkPolyData *input = this->GetInput();

	vtkPolyDataConnectivityFilter *connectivityFilter = vtkPolyDataConnectivityFilter::New();
	connectivityFilter->SetInput(input);
	connectivityFilter->SetExtractionModeToClosestPointRegion ();
	connectivityFilter->SetClosestPoint(Point);
	connectivityFilter->Modified();
	connectivityFilter->Update();

	output->DeepCopy(vtkPolyData::SafeDownCast(connectivityFilter->GetOutput()));

	connectivityFilter->Delete();
}
//----------------------------------------------------------------------------
void vtkHoleConnectivity::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
	vtkPolyDataToPolyDataFilter::PrintSelf(os,indent);
}

