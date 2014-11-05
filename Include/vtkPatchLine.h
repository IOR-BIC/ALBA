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

#include "vtkMEDConfigure.h"
#include "vtkPolyDataToPolyDataFilter.h"

class VTK_vtkMED_EXPORT vtkPatchLine : public vtkPolyDataToPolyDataFilter
{
	public:
	/** RTTI macro*/
	vtkTypeRevisionMacro(vtkPatchLine, vtkPolyDataToPolyDataFilter);

	/** return object instance */
		static vtkPatchLine *New();

public:		
	/** execute method */
	/*virtual*/ void ExecuteData(vtkDataObject *output);

protected:  
	vtkPatchLine() {};
	~vtkPatchLine() {};

private:
	vtkPatchLine(const vtkPatchLine&);  // Not implemented.
	void operator=(const vtkPatchLine&);  // Not implemented.
};

#endif
