/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewRXRotatedVolumesDebugger.h,v $
  Language:  C++
  Date:      $Date: 2009-10-05 13:03:44 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __medViewRXRotatedVolumesDebugger_H__
#define __medViewRXRotatedVolumesDebugger_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
#include "mafMatrix.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class mafGUI;

//----------------------------------------------------------------------------
// medViewRXRotatedVolumesDebugger :
//----------------------------------------------------------------------------
/** 
medViewRXRotatedVolumesDebugger is a View that visualize volume as projection along x or y axis and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class medViewRXRotatedVolumesDebugger: public mafViewVTK
{
public:
  medViewRXRotatedVolumesDebugger(wxString label = "RX", int camera_position = CAMERA_RX_FRONT, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  virtual ~medViewRXRotatedVolumesDebugger(); 

  mafTypeMacro(medViewRXRotatedVolumesDebugger, mafViewVTK);

  virtual mafView*  Copy(mafObserver *Listener);
  virtual void			OnEvent(mafEventBase *maf_event);

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
  Set/Get the WindowLevel Lookup table values.*/
  void SetLutRange(double low_val, double high_val);
  void GetLutRange(double minMax[2]);

  /** 
  Return true if a there is a volume inside the view.*/
  bool VolumeIsVisible() {return m_CurrentVolume != NULL;};

  void CameraUpdate();

protected:

  void CameraUpdateForRotatedVolumes();
  
  void SetCameraParallelToDataSetLocalAxis( int axis );

  virtual mafGUI *CreateGui();  
  mafSceneNode *m_CurrentVolume; ///< Current visualized volume
  
  mafMatrix m_OldABSPose;
  mafMatrix m_NewABSPose;


};
#endif
