/*=========================================================================

 Program: MAF2
 Module: mafViewSlice
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafViewSliceSegmentation_H__
#define __mafViewSliceSegmentation_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewSlice.h"
#include "mafMatrix.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;

class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// mafViewSliceSegmentation :
//----------------------------------------------------------------------------
/** 
mafViewSliceSegmentation is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor.
Rotated volumes interaction and visualization is supported by this component.
*/

class MAF_EXPORT mafViewSliceSegmentation: public mafViewSlice
{
public:
  mafViewSliceSegmentation(wxString label = "SliceSegmentation", int camera_position = CAMERA_CT, bool show_axes = false, bool show_grid = false, int stereo = 0,bool showTICKs=false,bool textureInterpolate=true);
  virtual ~mafViewSliceSegmentation(); 

  mafTypeMacro(mafViewSliceSegmentation, mafViewSlice);

  virtual mafView*  Copy(mafObserver *Listener, bool lightCopyEnabled = false);

	

	void VmeSegmentationShow(mafVME *vme, bool show);

	/**
	Create the visual pipe for the vme passed as argument.
	To create visual pipe first check in m_PipeMap if custom visual pipe is defined,
	otherwise ask to vme which is its visual pipe. */
	virtual void VmeCreatePipe(mafVME *vme);

	/** Delete vme's visual pipe. It is called when vme is removed from visualization.*/
	virtual void VmeDeletePipe(mafVME *vme);

	virtual void SetSliceAxis(int sliceAxis);
	/** Set the origin and normal of the slice
	Both, Origin and Normal may be NULL, if the current value is to be preserved */
	virtual void SetSlice(double* Origin = NULL, double* Normal = NULL);

protected:

	/** Create Pipe for Segmentation  */
	void VmeCreatePipeForSegmentation(mafVME *vme);
		
  mafSceneNode    *m_CurrentSegmentation; ///< Current visualized volume
	bool						m_IsShowingSegmentation;
};
#endif
