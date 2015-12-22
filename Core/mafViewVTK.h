/*=========================================================================

 Program: MAF2
 Module: mafViewVTK
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafViewVTK_H__
#define __mafViewVTK_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"
#include "mafRWIBase.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h" //used in subclasses
#include <map>
#include "mafAxes.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafLightKit;
class mafVME;
class mafDeviceButtonsPadMouse;
class vtkRenderWindow;
class vtkRenderer;
class mafAttachCamera;
class mafTextKit;
class mafAnimate;

//----------------------------------------------------------------------------
// mafViewVTK :
//----------------------------------------------------------------------------
/** 
mafViewVTK is a View that got a RenderWindow and a SceneGraph
\sa mafSceneNode mafRWIBase mafRWI
*/
class MAF_EXPORT mafViewVTK: public mafView
{
public:

  /** constructor */
  mafViewVTK(const wxString &label = "vtkView", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = true, bool show_grid = false, int stereo = 0, bool show_orientator = false, int axesType = mafAxes::TRIAD);
  /** constructor */
  virtual ~mafViewVTK(); 
  /** RTTI macro */
  mafTypeMacro(mafViewVTK, mafView);

  /** clone the object*/
  virtual mafView*  Copy(mafObserver *Listener, bool lightCopyEnabled = false);
  /** create rwi */
  virtual void Create();
  /** listen to other object events*/
  virtual void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum VIEW_VTK_WIDGET_ID
  {
    ID_ROLLOUT_ATTACH_CAMERA = Superclass::ID_LAST,
    ID_ROLLOUT_TEXT_KIT,
    ID_ROLLOUT_LIGHT_KIT,
    ID_ROLLOUT_ANIMATE_KIT,
    ID_LAST
  };

  /** Add the vme to the view's scene-graph*/
  virtual void VmeAdd(mafNode *vme);
  
  /** Remove the vme from the view's scene-graph*/
  virtual void VmeRemove(mafNode *vme);

  virtual void VmeSelect(mafNode *vme, bool select);

  /** Called to show/hide vme*/
  virtual void VmeShow(mafNode *vme, bool show);

  /** 
  Called to update visual pipe properties of the vme passed as argument. If the 'fromTag' flag is true,
  the update is done by reading the visual parameters from tags.*/
  virtual void VmeUpdateProperty(mafNode *vme, bool fromTag = false);

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph*/
  virtual int GetNodeStatus(mafNode *vme);
  
  /** 
  Return a pointer to the visual pipe of the node passed as argument. 
  It is used in mafSideBar to plug the visual pipe's GUI in the tabbed vme panel. \sa mafSideBar*/
  virtual mafPipe* GetNodePipe(mafNode *vme);
  
  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);
  
  /** Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

	virtual void CameraReset(mafNode *node = NULL);
  virtual void CameraSet(int camera_position);
  virtual void CameraUpdate();
  virtual int GetCameraPosition();

  virtual mafSceneGraph *GetSceneGraph()	  {return m_Sg;}; 
  virtual mafRWIBase    *GetRWI()           {return m_Rwi->m_RwiBase;};

  /** Return a pointer to the image of the renderwindow.*/
  void GetImage(wxBitmap &bmp, int magnification = 1);

  virtual vtkRenderer     *GetFrontRenderer();
  virtual vtkRenderer     *GetBackRenderer();
  virtual vtkRenderWindow *GetRenderWindow();

  /** 
  Find the pocked VME at button down. As argument the function needs
  the device which performed the action, and provides as result pointers
  to piked prop, vme and its behavior if it exists. */
  virtual bool FindPokedVme(mafDevice *device,mafMatrix &point_pose,vtkProp3D *&picked_prop,mafVME *&picked_vme,mafInteractor *&picked_behavior);

  /** Perform a picking according to the screen position (x,y) and return true on success*/
  virtual bool Pick(int x, int y);

  /** Perform a picking according to the absolute matrix given and return true on success*/
  virtual bool Pick(mafMatrix &m);

	/** Set the vtk RenderWindow size. Used only for Linux (not necessary for Windows) */
  void SetWindowSize(int w, int h);

  /** Struct containing information regarding visual pipe plugged into the view. */
  struct mafVisualPipeInfo
  {
    mafString m_PipeName;
    long      m_Visibility;
  };
  typedef std::map<mafString, mafVisualPipeInfo> mafPipeMap;

  /** Plug a visual pipe for a particular vme. It is used also to plug custom pipe.*/
  void PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility = VISIBLE);

  mafPipeMap m_PipeMap; ///< Map used to store visual pipeline associated with vme types

  /** Set the mouse device to use inside the view */
  void SetMouse(mafDeviceButtonsPadMouse *mouse);

  /** Print this view.*/
  virtual void Print(wxDC *dc, wxRect margins);

  /** Allow to link the camera of the view to other cameras.*/
  virtual void LinkView(bool link_camera = true);

  mafRWI *m_Rwi;

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

protected:

  mafSceneGraph *m_Sg;
  mafLightKit		*m_LightKit;
  mafTextKit    *m_TextKit;
  mafAttachCamera *m_AttachCamera;
  mafAnimate      *m_AnimateKit;

  int   m_CameraPositionId; ///< Integer representing a preset for camera position, focal point and view up.
  bool  m_ShowAxes;  ///< Flag used to show/hide axes in low left corner of the view
  int   m_StereoType; ///< Indicate the stereo type to use with the view
  bool  m_ShowGrid;
  bool m_ShowOrientator;
  int  m_AxesType;

  virtual mafGUI *CreateGui();

  virtual void OnPreResetCamera();
  virtual void OnPostResetCamera();

  /** Return the visual pipe's name.*/
  void GetVisualPipeName(mafNode *node, mafString &pipe_name);
};
#endif
