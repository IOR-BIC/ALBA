/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewImage
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaViewImage_H__
#define __albaViewImage_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewVTK.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaSceneNode;
class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// albaViewImage :
//----------------------------------------------------------------------------
/** 
albaViewImage is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa albaViewVTK
*/
class ALBA_EXPORT albaViewImage: public albaViewVTK
{
public:
  albaViewImage(wxString label = "Image", int camera_position = CAMERA_FRONT, bool show_axes = false, bool show_grid = false, int stereo = 0);
  virtual ~albaViewImage(); 

  albaTypeMacro(albaViewImage, albaViewVTK);

  virtual albaView*  Copy(albaObserver *Listener, bool lightCopyEnabled = false);
  virtual void			OnEvent(albaEventBase *alba_event);

  void Create();

  /** 
  Set the visualization status for the vme (visible, not visible, mutex, ...) \sa albaSceneGraph albaView*/
  virtual int GetNodeStatus(albaVME *vme);

  /** IDs for the view GUI */
  enum VIEW_SLICE_WIDGET_ID
  {
    ID_NONE = Superclass::ID_LAST,
    ID_LAST
  };
	  
  /** Create the background for the slice. */
  void BorderCreate(double col[3]);

  /** Delete the background of the slice. */
  void BorderDelete();

  void VmeShow(albaVME *vme, bool show);

  void VmeDeletePipe(albaVME *vme);

protected:
  virtual albaGUI *CreateGui();

  albaSceneNode    *m_CurrentImage; ///< Current visualized volume

};
#endif
