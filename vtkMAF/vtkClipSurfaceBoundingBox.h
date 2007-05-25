/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkClipSurfaceBoundingBox.h,v $
Language:  C++
Date:      $Date: 2007-05-25 14:31:41 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __vtkClipSurfaceBoundingBox_h
#define __vtkClipSurfaceBoundingBox_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "vtkMAFConfigure.h"
#include "vtkPolyData.h"
#include "vtkPolyDataToPolyDataFilter.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// vtkClipSurfaceBoundingBox :
//----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkClipSurfaceBoundingBox : public vtkPolyDataToPolyDataFilter 
{

public:

	static vtkClipSurfaceBoundingBox *New() {return new vtkClipSurfaceBoundingBox;};

	vtkTypeMacro(vtkClipSurfaceBoundingBox,vtkObject);

	void SetMask(vtkPolyData *mask) {this->SetNthInput(1, mask);};
	vtkPolyData *GetMask() { return (vtkPolyData *)(this->Inputs[1]);};

	vtkSetMacro(ClipInside,int);
	vtkGetMacro(ClipInside,int);

protected:
	vtkClipSurfaceBoundingBox();
	~vtkClipSurfaceBoundingBox();

	void Execute();

	int ClipInside;
};

#endif