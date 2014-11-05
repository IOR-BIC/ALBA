/*=========================================================================

 Program: MAF2Medical
 Module: mafViewImage
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafViewImage_H__
#define __mafViewImage_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medViewsDefines.h"
#include "mafViewVTK.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// mafViewImage :
//----------------------------------------------------------------------------
/** 
mafViewImage is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class MED_VIEWS_EXPORT mafViewImage: public mafViewVTK
{
public:
  mafViewImage(wxString label = "Image", int camera_position = CAMERA_FRONT, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  virtual ~mafViewImage(); 

  mafTypeMacro(mafViewImage, mafViewVTK);

  virtual mafView*  Copy(mafObserver *Listener, bool lightCopyEnabled = false);
  virtual void			OnEvent(mafEventBase *maf_event);

  void Create();

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph mafView*/
  virtual int GetNodeStatus(mafNode *vme);

  /** IDs for the view GUI */
  enum VIEW_SLICE_WIDGET_ID
  {
    ID_NONE = Superclass::ID_LAST,
    ID_LAST
  };

  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  //virtual void VmeCreatePipe(mafNode *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  //virtual void VmeDeletePipe(mafNode *vme);

  /** Create the background for the slice. */
  void BorderCreate(double col[3]);

  /** Delete the background of the slice. */
  void BorderDelete();

  void VmeShow(mafNode *node, bool show);

  void VmeDeletePipe(mafNode *vme);

protected:
  virtual mafGUI *CreateGui();

  mafSceneNode    *m_CurrentImage; ///< Current visualized volume

};
#endif
