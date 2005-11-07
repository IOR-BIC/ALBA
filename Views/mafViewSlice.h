/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSlice.h,v $
  Language:  C++
  Date:      $Date: 2005-11-07 13:31:19 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafViewSlice_H__
#define __mafViewSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
#include "mafPipeVolumeSlice.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;

//----------------------------------------------------------------------------
// mafViewSlice :
//----------------------------------------------------------------------------
/** 
mafViewSlice is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class mafViewSlice: public mafViewVTK
{
public:
  mafViewSlice(wxString label = "Slice", int camera_position = CAMERA_CT, bool show_axes = false, int stereo = 0, bool external = false);
  virtual ~mafViewSlice(); 

  mafTypeMacro(mafViewSlice, mafViewVTK);

  virtual mafView*  Copy(mafObserver *Listener);
  virtual void			OnEvent(mafEventBase *maf_event);

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph*/
  virtual int GetNodeStatus(mafNode *vme);

  /** IDs for the GUI */
/*  enum VIEW_VTK_WIDGET_ID
  {
    ID_ATTACH_CAMERA = Superclass::ID_LAST,
    ID_LAST
  };
*/

  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

  /** 
  Change the range of the WindowLevel Lookup table.*/
  void SetLutRange(double low_val, double high_val);

  /** 
  Return true if a there is a volume inside the view.*/
  bool VolumeIsVisible() {return m_CurrentVolume != NULL;};

protected:
  virtual mmgGui *CreateGui();
  mafSceneNode *m_CurrentVolume; ///< Current visualized volume
};
#endif
