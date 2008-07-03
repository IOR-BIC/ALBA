/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkHoleConnectivity.h
Language:  C++
Version:   $Id: vtkHoleConnectivity.h,v 1.3 2008-07-03 12:09:26 ior02 Exp $

Copyright (c) Goodwin Lawlor 2003-2004
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notice for more information.

Some modifications by Matteo Giacomoni in order to make it work
under MAF (www.openmaf.org)

=========================================================================*/
#ifndef __vtkHoleConnectivity_h
#define __vtkHoleConnectivity_h

#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkMAFConfigure.h"

class VTK_vtkMAF_EXPORT vtkHoleConnectivity : public vtkPolyDataToPolyDataFilter
{
public:

	const char *GetClassName() {return "vtkHoleConnectivity";};
	void PrintSelf(ostream& os, vtkIndent indent); 

	static vtkHoleConnectivity *New() {return new vtkHoleConnectivity;};

	vtkSetMacro(PointID,vtkIdType);

	vtkSetVector3Macro(point,double);

protected:
	vtkHoleConnectivity(vtkPolyData *input=NULL,vtkIdType ID=-1);
	~vtkHoleConnectivity();

	void Execute();

	vtkIdType PointID;
	double point[3];
};

#endif


