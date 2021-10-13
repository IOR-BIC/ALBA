/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewRX
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaViewRX_H__
#define __albaViewRX_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewVTK.h"
#include "albaMatrix.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaSceneNode;
class albaGUI;

enum VOLUME_SIDE
{
	ALL = 0,
	SIDE_LEFT,
	SIDE_RIGHT,
};

//----------------------------------------------------------------------------
// albaViewRX :
//----------------------------------------------------------------------------
/** 
albaViewRX is a View that visualize volume as projection along x or y axis and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa albaViewVTK
*/
class ALBA_EXPORT albaViewRX: public albaViewVTK
{
public:
  /** constructor */
  albaViewRX(wxString label = "RX", int camera_position = CAMERA_RX_FRONT, bool show_axes = false, bool show_grid = false, int stereo = 0);
  /** destructor*/
  virtual ~albaViewRX(); 

  /** RTTI macro*/
  albaTypeMacro(albaViewRX, albaViewVTK);

  /** clone an instance of the object*/
  virtual albaView*  Copy(albaObserver *Listener, bool lightCopyEnabled = false);

  /** Precess events coming from other objects */
  virtual void			OnEvent(albaEventBase *alba_event);

  /** create render window interactor, picker and scenegraph */
  void Create();

  /** 
  Set the visualization status for the vme (visible, not visible, mutex, ...) 
  \sa albaSceneGraph albaView*/
  virtual int GetNodeStatus(albaVME *vme);

  /** IDs for the GUI */
  enum VIEW_RX_WIDGET_ID
  {
    ID_NONE = Superclass::ID_LAST,
    ID_LAST
  };

  /** 
  Create the visual pipe for the vme passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(albaVME *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(albaVME *vme);

  /** 
  Set the WindowLevel Lookup table values.*/
  void SetLutRange(double low_val, double high_val);
  /** 
  Get the WindowLevel Lookup table values.*/
  void GetLutRange(double minMax[2]);

  /** 
  Return true if a there is a volume inside the view.*/
  bool VolumeIsVisible() {return m_CurrentVolume != NULL;};

	void ShowSideVolume(VOLUME_SIDE side);

  /** Update camera view*/
  void CameraUpdate();

protected:

  /** update camera view for volume with a specific rotation*/
  void CameraUpdateForRotatedVolumes();

	/** set camera direction parallel to one of the axis of the dataset */
  void SetCameraParallelToDataSetLocalAxis( int axis );

  /** create gui widgets */
  virtual albaGUI *CreateGui();  
  albaSceneNode *m_CurrentVolume; ///< Current visualized volume
	VOLUME_SIDE m_CurrentSide;

  albaMatrix m_OldABSPose;
  albaMatrix m_NewABSPose;


};
#endif
