/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkHoleConnectivity.h
Language:  C++
Version:   $Id: vtkHoleConnectivity.h,v 1.4.2.2 2009-11-23 10:49:00 ior04 Exp $

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
/**
  class name: vtkHoleConnectivity
  This filter uses vtkPolyDataConnectivityFilter in order to extract the Closest Point Region
  after pass it the coordinates of the point.
*/
class VTK_vtkMAF_EXPORT vtkHoleConnectivity : public vtkPolyDataToPolyDataFilter
{
public:
  /** retrieve class name */
	const char *GetClassName() {return "vtkHoleConnectivity";};
  /** print information */
	void PrintSelf(ostream& os, vtkIndent indent); 

  /** create an instance of the object */
	static vtkHoleConnectivity *New() {return new vtkHoleConnectivity;};

  /** macro for setting point id */
	vtkSetMacro(PointID,vtkIdType);
  /** macro for getting point id */
  vtkGetMacro(PointID,vtkIdType);
  /** macro for setting point coordinates */
	vtkSetVector3Macro(Point,double);
  /** macro for getting point coordinates */
  vtkGetVector3Macro(Point,double);

protected:
  /** constructor */
	vtkHoleConnectivity(vtkPolyData *input=NULL,vtkIdType ID=-1);
  /** destructor */
	~vtkHoleConnectivity();

  /** execute the filter */
	void Execute();

	vtkIdType PointID;
	double Point[3];
};

#endif


