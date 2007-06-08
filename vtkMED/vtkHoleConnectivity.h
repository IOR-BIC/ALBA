/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkRemoveCellsFilter.h
Language:  C++
Version:   $Id: vtkHoleConnectivity.h,v 1.2 2007-06-08 07:55:55 ior02 Exp $

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


