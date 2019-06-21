/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAClipSurfaceBoundingBox
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAClipSurfaceBoundingBox_h
#define __vtkALBAClipSurfaceBoundingBox_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaConfigure.h"
#include "vtkPolyData.h"
#include "vtkPolyDataToPolyDataFilter.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

/** 
  Class Name: vtkALBAClipSurfaceBoundingBox.
  Class used for clip an input surface with a box generated with the extrusion for example of a plane.
  The ClipInside flag is used to retrieve one of the two parts in which the input surface is clipped.
*/
class ALBA_EXPORT vtkALBAClipSurfaceBoundingBox : public vtkPolyDataToPolyDataFilter 
{

public:
  /** static function for creating object. */
	static vtkALBAClipSurfaceBoundingBox *New();

  /** RTTI macro*/
	vtkTypeRevisionMacro(vtkALBAClipSurfaceBoundingBox,vtkObject);

  /** Set the polydata with which clip is performed.*/
	void SetMask(vtkPolyData *mask) {this->SetNthInput(1, mask);};
  /** Retrieve the mask polydata.*/
	vtkPolyData *GetMask() { return (vtkPolyData *)(this->Inputs[1]);};

  /** Set macro for ClipInside.*/
	vtkSetMacro(ClipInside,int);
  /** Get macro for ClipInside.*/
	vtkGetMacro(ClipInside,int);

protected:
  /** constructor*/
	vtkALBAClipSurfaceBoundingBox();
  /** destructor */
	~vtkALBAClipSurfaceBoundingBox();

  /** Execute the filter. */
	void Execute();

	int ClipInside;
};

#endif
