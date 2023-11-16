/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewVTK
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaViewVTK_H__
#define __albaViewVTK_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaView.h"
#include "albaRWIBase.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h" //used in subclasses
#include <map>
#include "albaAxes.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaLightKit;
class albaVME;
class albaDeviceButtonsPadMouse;
class vtkRenderWindow;
class vtkRenderer;
class albaAttachCamera;
class albaTextKit;
class albaAnimate;

//----------------------------------------------------------------------------
// albaViewVTK :
//----------------------------------------------------------------------------
/** 
albaViewVTK is a View that got a RenderWindow and a SceneGraph
\sa albaSceneNode albaRWIBase albaRWI
*/
class ALBA_EXPORT albaViewVTK: public albaView
{
public:

  /** constructor */
  albaViewVTK(const wxString &label = "vtkView", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = true, bool show_grid = false, int stereo = 0, bool show_orientator = false, int axesType = albaAxes::TRIAD);
  /** constructor */
  virtual ~albaViewVTK(); 
  /** RTTI macro */
  albaTypeMacro(albaViewVTK, albaView);

  /** clone the object*/
  virtual albaView*  Copy(albaObserver *Listener, bool lightCopyEnabled = false);
  /** create rwi */
  virtual void Create();
  /** listen to other object events*/
  virtual void OnEvent(albaEventBase *alba_event);

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
  virtual void VmeAdd(albaVME *vme);
  
  /** Remove the vme from the view's scene-graph*/
  virtual void VmeRemove(albaVME *vme);

  virtual void VmeSelect(albaVME *vme, bool select);

  /** Called to show/hide vme*/
  virtual void VmeShow(albaVME *vme, bool show);

  /** 
  Called to update visual pipe properties of the vme passed as argument. If the 'fromTag' flag is true,
  the update is done by reading the visual parameters from tags.*/
  virtual void VmeUpdateProperty(albaVME *vme, bool fromTag = false);

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa albaSceneGraph*/
  virtual int GetNodeStatus(albaVME *vme);
  
  /** 
  Return a pointer to the visual pipe of the node passed as argument. 
  It is used in albaSideBar to plug the visual pipe's GUI in the tabbed vme panel. \sa albaSideBar*/
  virtual albaPipe* GetNodePipe(albaVME *vme);
  
  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(albaVME *vme);
  
  /** Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(albaVME *vme);

	virtual void CameraReset(albaVME *vme = NULL);
  virtual void CameraSet(int camera_position);
  virtual void CameraUpdate();
  virtual int GetCameraPosition();

	virtual void SetBackgroundColor(wxColor color);

	virtual albaSceneGraph *GetSceneGraph() { return m_Sg; };
  virtual albaRWIBase    *GetRWI()           {return m_Rwi->m_RwiBase;};

  /** Return a pointer to the image of the renderwindow.*/
  void GetImage(wxBitmap &bmp, int magnification = 1);

  virtual vtkRenderer     *GetFrontRenderer();
  virtual vtkRenderer     *GetBackRenderer();
  virtual vtkRenderWindow *GetRenderWindow();

  /** 
  Find the pocked VME at button down. As argument the function needs
  the device which performed the action, and provides as result pointers
  to piked prop, vme and its behavior if it exists. */
  virtual bool FindPokedVme(albaDevice *device,albaMatrix &point_pose,vtkProp3D *&picked_prop,albaVME *&picked_vme,albaInteractor *&picked_behavior);

  /** Perform a picking according to the screen position (x,y) and return true on success*/
  virtual bool Pick(int x, int y);

  /** Perform a picking according to the absolute matrix given and return true on success*/
  virtual bool Pick(albaMatrix &m);

	/** Set the vtk RenderWindow size. Used only for Linux (not necessary for Windows) */
  void SetWindowSize(int w, int h);

  /** Struct containing information regarding visual pipe plugged into the view. */
  struct albaVisualPipeInfo
  {
    albaString m_PipeName;
    long      m_Visibility;
  };
  typedef std::map<albaString, albaVisualPipeInfo> albaPipeMap;

  /** Plug a visual pipe for a particular vme. It is used also to plug custom pipe.*/
  void PlugVisualPipe(albaString vme_type, albaString pipe_type, long visibility = VISIBLE);

  albaPipeMap m_PipeMap; ///< Map used to store visual pipeline associated with vme types

  /** Set the mouse device to use inside the view */
  void SetMouse(albaDeviceButtonsPadMouse *mouse);
  
  albaRWI *m_Rwi;

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

protected:

  albaSceneGraph *m_Sg;
  albaLightKit		*m_LightKit;
  albaTextKit    *m_TextKit;
  albaAttachCamera *m_AttachCamera;
  albaAnimate      *m_AnimateKit;

  int   m_CameraPositionId; ///< Integer representing a preset for camera position, focal point and view up.
  bool  m_ShowAxes;  ///< Flag used to show/hide axes in low left corner of the view
  int   m_StereoType; ///< Indicate the stereo type to use with the view
  bool  m_ShowGrid;
  bool m_ShowOrientator;
  int  m_AxesType;

  virtual albaGUI *CreateGui();

  virtual void OnPreResetCamera();
  virtual void OnPostResetCamera();

  /** Return the visual pipe's name.*/
  virtual void GetVisualPipeName(albaVME *node, albaString &pipe_name);
};
#endif
