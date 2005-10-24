/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-10-24 10:53:18 $
  Version:   $Revision: 1.25 $
  Authors:   Silvano Imboden
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
class vtkMatrix4x4;
class mmdMouse;
class vtkRenderWindow;
class vtkRenderer;
class vtkCellPicker;
class vtkRayCast3DPicker;
class vtkAssemblyPath;

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
  enum VIEW_VTK_WIDGET_ID
  {
    ID_ATTACH_CAMERA = Superclass::ID_LAST,
    ID_LAST
  };

  virtual void			VmeAdd(mafNode *vme);
  virtual void			VmeRemove(mafNode *vme);
  virtual void			VmeSelect(mafNode *vme, bool select);
  virtual void			VmeShow(mafNode *vme, bool show);
	virtual void      VmeUpdateProperty(mafNode *vme, bool fromTag = false);
  virtual int 	    GetNodeStatus(mafNode *vme);
  virtual mafPipe*  GetNodePipe(mafNode *vme);
  virtual void	    VmeCreatePipe(mafNode *vme);
  virtual void	    VmeDeletePipe(mafNode *vme);

	virtual void			CameraReset(mafNode *node = NULL);
  virtual void      CameraSet(int camera_position);
  virtual void			CameraUpdate();

  virtual mafSceneGraph *GetSceneGraph()	  {return m_Sg;}; 
  virtual mafRWIBase    *GetRWI()           {return m_Rwi->m_RwiBase;};

  virtual vtkRenderer     *GetFrontRenderer();
  virtual vtkRenderer     *GetBackRenderer();
  virtual vtkRenderWindow *GetRenderWindow();

  /** 
  Perform a picking according to the screen position (x,y) and return true on success*/
  bool Pick(int x, int y);

  /** 
  Perform a picking according to the absolute matrix given and return true on success*/
  bool Pick(mafMatrix &m);

  /** 
  Write into the double array the position picked during Pick method.*/
  void GetPickedPosition(double pos[3]);

  /** 
  Return the picked VME during the Pick method. Return NULL if VME is not found*/
  mafVME *GetPickedVme() {return m_PickedVME;};
	
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
  int            m_CameraPosition; 
  int						 m_CameraAttach; ///< Flag to turn On/Off the camera attaching on a particular VME
  mafVME				*m_AttachedVme; ///< VME on which the camera is attached when the attach-camera option is 'On'
  vtkMatrix4x4	*m_AttachedVmeMatrix; ///< Matrix given to the Camera to be moved together with m_AttachedVme

  vtkCellPicker *m_Picker2D;  ///< the picker used to pick the in the render window
  vtkRayCast3DPicker* m_Picker3D; ///< Used to pick in a VTK Render window
  mafVME        *m_PickedVME; ///< Pointer to the picked vme. It is initialized on picking
  double        m_PickedPosition[3];

  bool m_ShowAxes;  ///< Flag used to show/hide axes in low left cornel of the view
  int m_StereoType; ///< Indicate the stereo type to use with the view

  /** 
  Find the VME picked */
  void FindPickedVme(vtkAssemblyPath *ap = NULL);

  virtual mmgGui *CreateGui();

  virtual void OnPreResetCamera();
  virtual void OnPostResetCamera();

  /** 
  Update the camera position when vme's abs matrix change. 
  This function is called from CameraUpdate when camera is attached to the vme */
  void UpdateCameraMatrix();
};
#endif
