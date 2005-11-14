/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-11-14 16:54:46 $
  Version:   $Revision: 1.32 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafLightKit;
class mafVME;
class mmdMouse;
class vtkRenderWindow;
class vtkRenderer;
class mafAttachCamera;
class mafTextKit;

/** VME visibility attribute when plugging visual pipe.*/
enum VME_VISIBILITY_ID
{
  NON_VISIBLE = 0,
  VISIBLE,
  MUTEX,
};

//----------------------------------------------------------------------------
// mafViewVTK :
//----------------------------------------------------------------------------
/** 
mafViewVTK is a View that got a RenderWindow and a SceneGraph
\sa mafSceneNode mafRWIBase mafRWI
*/
class mafViewVTK: public mafView
{
public:
  mafViewVTK(wxString label = "vtkView", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = true, int stereo = 0, bool external = false);
  virtual ~mafViewVTK(); 

  mafTypeMacro(mafViewVTK, mafView);

  virtual mafView*  Copy(mafObserver *Listener);
  virtual void      Create();
  virtual void			OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
/*  enum VIEW_VTK_WIDGET_ID
  {
    ID = Superclass::ID_LAST,
    ID_LAST
  };
*/
  /** 
  Add the vme to the view's scene-graph*/
  virtual void VmeAdd(mafNode *vme);
  /** 
  Remove the vme from the view's scene-graph*/
  virtual void VmeRemove(mafNode *vme);

  virtual void VmeSelect(mafNode *vme, bool select);

  /** 
  Called to show/hide vme*/
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
  virtual mafPipe*  GetNodePipe(mafNode *vme);
  
  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);
  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

	virtual void CameraReset(mafNode *node = NULL);
  virtual void CameraSet(int camera_position);
  virtual void CameraUpdate();

  virtual mafSceneGraph *GetSceneGraph()	  {return m_Sg;}; 
  virtual mafRWIBase    *GetRWI()           {return m_Rwi->m_RwiBase;};

  virtual vtkRenderer     *GetFrontRenderer();
  virtual vtkRenderer     *GetBackRenderer();
  virtual vtkRenderWindow *GetRenderWindow();

  /** 
  Perform a picking according to the screen position (x,y) and return true on success*/
  virtual bool Pick(int x, int y);

  /** 
  Perform a picking according to the absolute matrix given and return true on success*/
  virtual bool Pick(mafMatrix &m);

	/** 
    Set the vtk RenderWindow size. Used only for Linux (not necessary for Windows) */
  void SetWindowSize(int w, int h);

  /** Struct containing information regarding visual pipe plugged into the view. */
  struct mafVisualPipeInfo
  {
    mafString m_PipeName;
    long      m_Visibility;
  };
  typedef std::map<mafString, mafVisualPipeInfo> mafPipeMap;

  /** 
    Plug a visual pipe for a particular vme. It is used also to plug custom pipe.*/
  void PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility = VISIBLE);

  mafPipeMap m_PipeMap; ///< Map used to store visual pipeline associated with vme types

  /** 
    Set the mouse device to use inside the view */
  void SetMouse(mmdMouse *mouse);

protected:
  mafRWI        *m_Rwi; 
  mafSceneGraph *m_Sg;
  mafLightKit		*m_LightKit;
  mafTextKit    *m_TextKit;
  mafAttachCamera *m_AttachCamera;
  int            m_CameraPosition; 

  int   m_NumberOfVisibleVme; ///< perform ResetCamera only for the first vme shown into the view
  bool  m_ShowAxes;  ///< Flag used to show/hide axes in low left cornel of the view
  int   m_StereoType; ///< Indicate the stereo type to use with the view

  virtual mmgGui *CreateGui();

  virtual void OnPreResetCamera();
  virtual void OnPostResetCamera();

  /** 
  Return the visual pipe's name.*/
  void GetVisualPipeName(mafNode *node, mafString &pipe_name);
};
#endif
