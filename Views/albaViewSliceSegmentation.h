/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewSlice
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaViewSliceSegmentation_H__
#define __albaViewSliceSegmentation_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewSlice.h"
#include "albaMatrix.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaSceneNode;

class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// albaViewSliceSegmentation :
//----------------------------------------------------------------------------
/** 
albaViewSliceSegmentation is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor.
Rotated volumes interaction and visualization is supported by this component.
*/

class ALBA_EXPORT albaViewSliceSegmentation: public albaViewSlice
{
public:
  albaViewSliceSegmentation(wxString label = "SliceSegmentation", int camera_position = CAMERA_CT, bool show_axes = false, bool show_grid = false, int stereo = 0,bool showTICKs=false,bool textureInterpolate=true);
  virtual ~albaViewSliceSegmentation(); 

  albaTypeMacro(albaViewSliceSegmentation, albaViewSlice);

  virtual albaView*  Copy(albaObserver *Listener, bool lightCopyEnabled = false);
	
	virtual void VmeShow(albaVME *vme, bool show);

	void VmeSegmentationShow(albaVME *vme, bool show);

	/**
	Create the visual pipe for the vme passed as argument.
	To create visual pipe first check in m_PipeMap if custom visual pipe is defined,
	otherwise ask to vme which is its visual pipe. */
	virtual void VmeCreatePipe(albaVME *vme);

	/** Delete vme's visual pipe. It is called when vme is removed from visualization.*/
	virtual void VmeDeletePipe(albaVME *vme);

	virtual void SetSliceAxis(int sliceAxis);
	/** Set the origin and normal of the slice
	Both, Origin and Normal may be NULL, if the current value is to be preserved */
	virtual void SetSlice(double* Origin = NULL, double* Normal = NULL);

protected:

	/** Create Pipe for Segmentation  */
	void VmeCreatePipeForSegmentation(albaVME *vme);
		
  albaSceneNode    *m_CurrentSegmentation; ///< Current visualized volume
	bool						m_IsShowingSegmentation;
};
#endif
