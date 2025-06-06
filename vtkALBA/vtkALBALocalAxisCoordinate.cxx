/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBALocalAxisCoordinate
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkALBALocalAxisCoordinate.h"
#include "vtkViewport.h"
#include "vtkObjectFactory.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"

//vtkCxxRevisionMacro(vtkALBALocalAxisCoordinate, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkALBALocalAxisCoordinate);

#define VTK_RINT(x) ((x > 0.0) ? (int)(x + 0.5) : (int)(x - 0.5))

//----------------------------------------------------------------------------
vtkALBALocalAxisCoordinate::vtkALBALocalAxisCoordinate()
//----------------------------------------------------------------------------
{
  this->CoordinateSystem = VTK_USERDEFINED;
  this->DataSet = NULL;
  this->Matrix = NULL;
}
//----------------------------------------------------------------------------
vtkALBALocalAxisCoordinate::~vtkALBALocalAxisCoordinate()
//----------------------------------------------------------------------------
{
  this->SetDataSet(NULL);
	this->SetMatrix(NULL);
}
//----------------------------------------------------------------------------
double *vtkALBALocalAxisCoordinate::GetComputedUserDefinedValue(vtkViewport *viewport)
//----------------------------------------------------------------------------
{
  double size = 100;
	if(DataSet) size = DataSet->GetLength()/8.0;

  int   i;
	double v[4]; 
	for(i=0; i<3; i++) v[i] = this->Value[i] * size;
	v[3]=1;

	double w[4]; 
  vtkMatrix4x4 *m = Matrix;
	if(m)
	{
		w[0] = v[0]*m->Element[0][0] + v[1]*m->Element[0][1] +  v[2]*m->Element[0][2] + v[3]*m->Element[0][3];
		w[1] = v[0]*m->Element[1][0] + v[1]*m->Element[1][1] +  v[2]*m->Element[1][2] + v[3]*m->Element[1][3];
		w[2] = v[0]*m->Element[2][0] + v[1]*m->Element[2][1] +  v[2]*m->Element[2][2] + v[3]*m->Element[2][3];
		w[3] = 1;
  }
	else
  {
		for(i=0; i<3; i++) w[i] = v[i];
		w[3] = 1;
	}

  viewport->SetWorldPoint(w);
	viewport->WorldToDisplay();
	viewport->GetDisplayPoint(ComputedUserDefinedValue);  

  return this->ComputedUserDefinedValue;
}
