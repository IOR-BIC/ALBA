/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAStructuredPointsAlgorithm
 Authors: Gianluigi Crimi
 
 Copyright (c) IOR-LTM
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAStructuredPointsAlgorithm_h
#define __vtkALBAStructuredPointsAlgorithm_h

#include "albaConfigure.h"
#include "vtkDataSetAlgorithm.h"
/**
class name: vtkALBAStructuredPointsAlgorithm
 vtkALBAStructuredPointsAlgorithm is a filter whose subclasses take as
 input rectilinear grid datasets and generate polygonal data on output.
*/
class ALBA_EXPORT vtkALBAStructuredPointsAlgorithm : public vtkDataSetAlgorithm
{
public:
  /** create an instance of the object */
  static vtkALBAStructuredPointsAlgorithm *New();
  /** RTTI Macro */
  vtkTypeMacro(vtkALBAStructuredPointsAlgorithm,vtkDataSetAlgorithm);


protected:
	/** specialize output information type */
	virtual int FillOutputPortInformation(int port, vtkInformation* info);
	/** specialize input information type */
	virtual int FillInputPortInformation(int port, vtkInformation* info);

};

#endif





