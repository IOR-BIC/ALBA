/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFClipSurfaceBoundingBox.h,v $
Language:  C++
Date:      $Date: 2008-07-03 11:27:45 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __vtkMAFClipSurfaceBoundingBox_h
#define __vtkMAFClipSurfaceBoundingBox_h

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
// vtkMAFClipSurfaceBoundingBox :
//----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMAFClipSurfaceBoundingBox : public vtkPolyDataToPolyDataFilter 
{

public:

	static vtkMAFClipSurfaceBoundingBox *New();

	vtkTypeRevisionMacro(vtkMAFClipSurfaceBoundingBox,vtkObject);

	void SetMask(vtkPolyData *mask) {this->SetNthInput(1, mask);};
	vtkPolyData *GetMask() { return (vtkPolyData *)(this->Inputs[1]);};

	vtkSetMacro(ClipInside,int);
	vtkGetMacro(ClipInside,int);

protected:
	vtkMAFClipSurfaceBoundingBox();
	~vtkMAFClipSurfaceBoundingBox();

	void Execute();

	int ClipInside;
};

#endif
