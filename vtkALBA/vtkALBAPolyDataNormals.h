/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPolyDataNormals.h
 Authors: Josef Kohout (besoft@kiv.zcu.cz)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAPolyDataNormals_h
#define __vtkALBAPolyDataNormals_h

/**
Classname: vtkALBAPolyDataNormals - smart normals calculation for input data

This class calculates normals for the given input data
but unlike vtkPolyDataNormals, it does not recalculate them when nothing has changed 
*/

#include "vtkPolyDataNormals.h"

#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "albaConfigure.h"


class ALBA_EXPORT  vtkALBAPolyDataNormals : public vtkPolyDataNormals
{
public:
  vtkTypeMacro(vtkALBAPolyDataNormals, vtkPolyDataNormals);  
  static vtkALBAPolyDataNormals *New();

protected:
	unsigned long m_LastUpdateTime;	

protected:
  vtkALBAPolyDataNormals();
	
	//this is update method to avoid recalculation of things
	/*virtual*/ int RequestData(vtkInformation *request,	vtkInformationVector **inputVector,	vtkInformationVector *outputVector);
private:
  vtkALBAPolyDataNormals(const vtkALBAPolyDataNormals&);  // Not implemented.
  void operator=(const vtkALBAPolyDataNormals&);					// Not implemented.
};

#endif