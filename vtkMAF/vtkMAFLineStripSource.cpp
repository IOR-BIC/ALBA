/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFLineStripSource.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMAFLineStripSource.h"

#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"

#include <float.h>
#include <math.h>

vtkCxxRevisionMacro(vtkMAFLineStripSource, "$Revision: 1.43 $");
vtkStandardNewMacro(vtkMAFLineStripSource);

//----------------------------------------------------------------------------
vtkMAFLineStripSource::vtkMAFLineStripSource():vtkMAFAbstractPointsSource()
{
}

//----------------------------------------------------------------------------
vtkMAFLineStripSource::~vtkMAFLineStripSource()
{
}

//----------------------------------------------------------------------------
void vtkMAFLineStripSource::SetCellArrayToOutput(vtkPolyData *output, vtkCellArray *cellArray)
{
	output->SetLines(cellArray);
}

