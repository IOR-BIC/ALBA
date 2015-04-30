/*=========================================================================

 Program: MAF2
 Module: vtkMAFPolyDataNormals.h
 Authors: Josef Kohout (besoft@kiv.zcu.cz)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFPolyDataNormals_h
#define __vtkMAFPolyDataNormals_h

/**
Classname: vtkMAFPolyDataNormals - smart normals calculation for input data

This class calculates normals for the given input data
but unlike vtkPolyDataNormals, it does not recalculate them when nothing has changed 
*/

#include "vtkPolyDataNormals.h"

#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "mafConfigure.h"


class MAF_EXPORT  vtkMAFPolyDataNormals : public vtkPolyDataNormals
{
public:
  vtkTypeRevisionMacro(vtkMAFPolyDataNormals, vtkPolyDataNormals);  
  static vtkMAFPolyDataNormals *New();

protected:
	unsigned long m_LastUpdateTime;	

protected:
  vtkMAFPolyDataNormals();
	
	//this is update method to avoid recalculation of things
	/*virtual*/ void UpdateData(vtkDataObject *outputo);

private:
  vtkMAFPolyDataNormals(const vtkMAFPolyDataNormals&);  // Not implemented.
  void operator=(const vtkMAFPolyDataNormals&);					// Not implemented.
};

#endif