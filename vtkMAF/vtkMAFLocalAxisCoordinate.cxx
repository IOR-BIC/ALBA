/*=========================================================================

  Program:   Multimod Fundation Library
  Module:    $RCSfile: vtkMAFLocalAxisCoordinate.cxx,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:27:45 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden 
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================*/
#include "vtkMAFLocalAxisCoordinate.h"
#include "vtkViewport.h"
#include "vtkObjectFactory.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"

//vtkCxxRevisionMacro(vtkMAFLocalAxisCoordinate, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFLocalAxisCoordinate);

#define VTK_RINT(x) ((x > 0.0) ? (int)(x + 0.5) : (int)(x - 0.5))

//----------------------------------------------------------------------------
vtkMAFLocalAxisCoordinate::vtkMAFLocalAxisCoordinate()
//----------------------------------------------------------------------------
{
  this->CoordinateSystem = VTK_USERDEFINED;
  this->DataSet = NULL;
  this->Matrix = NULL;
}
//----------------------------------------------------------------------------
vtkMAFLocalAxisCoordinate::~vtkMAFLocalAxisCoordinate()
//----------------------------------------------------------------------------
{
  this->SetDataSet(NULL);
	this->SetMatrix(NULL);
}
//----------------------------------------------------------------------------
double *vtkMAFLocalAxisCoordinate::GetComputedUserDefinedValue(vtkViewport *viewport)
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
