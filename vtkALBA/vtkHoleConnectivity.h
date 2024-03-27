/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkHoleConnectivity.h
Language:  C++
Version:   $Id: vtkHoleConnectivity.h,v 1.4.2.3 2011-05-26 08:33:31 ior02 Exp $

Copyright (c) Goodwin Lawlor 2003-2004
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notice for more information.

Some modifications by Matteo Giacomoni in order to make it work
under ALBA

=========================================================================*/
#ifndef __vtkHoleConnectivity_h
#define __vtkHoleConnectivity_h

#define _WINSOCKAPI_ 
#include "vtkPolyDataConnectivityFilter.h"
#include "albaConfigure.h"
/**
  class name: vtkHoleConnectivity
  This filter uses vtkPolyDataConnectivityFilter in order to extract the Closest Point Region
  after pass it the coordinates of the point.
*/
class ALBA_EXPORT vtkHoleConnectivity : public vtkPolyDataConnectivityFilter
{
public:

  /** RTTI macro */
  vtkTypeMacro(vtkHoleConnectivity,vtkPolyDataConnectivityFilter);

  /** retrieve class name */
	const char *GetClassName() {return "vtkHoleConnectivity";};
  /** print information */
	void PrintSelf(ostream& os, vtkIndent indent); 

  /** create an instance of the object */
	static vtkHoleConnectivity *New();

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
	int RequestData(vtkInformation *request,	vtkInformationVector **inputVector,	vtkInformationVector *outputVector);

	vtkIdType PointID;
	double Point[3];
};

#endif


