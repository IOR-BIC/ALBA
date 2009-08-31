/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFClipSurfaceBoundingBox.h,v $
Language:  C++
Date:      $Date: 2009-08-31 12:49:15 $
Version:   $Revision: 1.1.2.1 $
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

/** 
  Class Name: vtkMAFClipSurfaceBoundingBox.
  Class used for clip an input surface with a box generated with the extrusion for example of a plane.
  The ClipInside flag is used to retrieve one of the two parts in which the input surface is clipped.
*/
class VTK_vtkMAF_EXPORT vtkMAFClipSurfaceBoundingBox : public vtkPolyDataToPolyDataFilter 
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
