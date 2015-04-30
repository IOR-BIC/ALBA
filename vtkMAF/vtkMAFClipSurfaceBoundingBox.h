/*=========================================================================

 Program: MAF2
 Module: vtkMAFClipSurfaceBoundingBox
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFClipSurfaceBoundingBox_h
#define __vtkMAFClipSurfaceBoundingBox_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafConfigure.h"
#include "vtkPolyData.h"
#include "vtkPolyDataToPolyDataFilter.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

/** 
  Class Name: vtkMAFClipSurfaceBoundingBox.
  Class used for clip an input surface with a box generated with the extrusion for example of a plane.
  The ClipInside flag is used to retrieve one of the two parts in which the input surface is clipped.
*/
class MAF_EXPORT vtkMAFClipSurfaceBoundingBox : public vtkPolyDataToPolyDataFilter 
{

public:
  /** static function for creating object. */
	static vtkMAFClipSurfaceBoundingBox *New();

  /** RTTI macro*/
	vtkTypeRevisionMacro(vtkMAFClipSurfaceBoundingBox,vtkObject);

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
	vtkMAFClipSurfaceBoundingBox();
  /** destructor */
	~vtkMAFClipSurfaceBoundingBox();

  /** Execute the filter. */
	void Execute();

	int ClipInside;
};

#endif
