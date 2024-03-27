/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkTriangleQualityRatio.cxx,v $
Language:  C++
Date:      $Date: 2011-05-26 08:33:31 $
Version:   $Revision: 1.4.2.2 $
Authors:   Gabriel Shanahan
==========================================================================
Copyright (c) 2012 University of West Bohemia (www.zcu.cz)
See the COPYINGS file for license details 
=========================================================================
*/
#ifndef __vtkPatchLine_h
#define __vtkPatchLine_h

#include "albaConfigure.h"
#include "vtkPolyDataAlgorithm.h"

class ALBA_EXPORT vtkPatchLine : public vtkPolyDataAlgorithm
{
	public:
	/** RTTI macro*/
	vtkTypeMacro(vtkPatchLine, vtkPolyDataAlgorithm);

	/** return object instance */
		static vtkPatchLine *New();

public:		
	/** execute method */
	int RequestData(vtkInformation *vtkNotUsed(request),	vtkInformationVector **inputVector,	vtkInformationVector *outputVector);

protected:  
	vtkPatchLine() {};
	~vtkPatchLine() {};

private:
	vtkPatchLine(const vtkPatchLine&);  // Not implemented.
	void operator=(const vtkPatchLine&);  // Not implemented.
};

#endif