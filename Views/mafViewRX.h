/*=========================================================================

 Program: MAF2Medical
 Module: mafViewRX
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafViewRX_H__
#define __mafViewRX_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medViewsDefines.h"
#include "mafViewVTK.h"
#include "mafMatrix.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class mafGUI;

//----------------------------------------------------------------------------
// mafViewRX :
//----------------------------------------------------------------------------
/** 
mafViewRX is a View that visualize volume as projection along x or y axis and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class MED_VIEWS_EXPORT mafViewRX: public mafViewVTK
{
public:
  /** constructor */
  mafViewRX(wxString label = "RX", int camera_position = CAMERA_RX_FRONT, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  /** destructor*/
  virtual ~mafViewRX(); 

  /** RTTI macro*/
  mafTypeMacro(mafViewRX, mafViewVTK);

  /** clone an instance of the object*/
  virtual mafView*  Copy(mafObserver *Listener, bool lightCopyEnabled = false);

  /** Precess events coming from other objects */
  virtual void			OnEvent(mafEventBase *maf_event);

  /** create render window interactor, picker and scenegraph */
  void Create();

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) 
  \sa mafSceneGraph mafView*/
  virtual int GetNodeStatus(mafNode *vme);

  //void VmeShow(mafNode *vme, bool show);

  /** IDs for the GUI */
  enum VIEW_RX_WIDGET_ID
  {
    ID_NONE = Superclass::ID_LAST,
    ID_LAST
  };

  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

  /** 
  Set the WindowLevel Lookup table values.*/
  void SetLutRange(double low_val, double high_val);
  /** 
  Get the WindowLevel Lookup table values.*/
  void GetLutRange(double minMax[2]);

  /** 
  Return true if a there is a volume inside the view.*/
  bool VolumeIsVisible() {return m_CurrentVolume != NULL;};

  /** Update camera view*/
  void CameraUpdate();

protected:

  /** update camera view for volume with a specific rotation*/
  void CameraUpdateForRotatedVolumes();
  
  /** set camera direction parallel to one of the axis of the dataset */
  void SetCameraParallelToDataSetLocalAxis( int axis );

  /** create gui widgets */
  virtual mafGUI *CreateGui();  
  mafSceneNode *m_CurrentVolume; ///< Current visualized volume
  
  mafMatrix m_OldABSPose;
  mafMatrix m_NewABSPose;


};
#endif
