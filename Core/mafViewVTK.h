/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-06-28 10:21:57 $
  Version:   $Revision: 1.12 $
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
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafLightKit;
class mafVME;
class vtkMatrix4x4;
class mmdMouse;

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
  mafViewVTK(wxString label = "vtkView", bool external = false);
  virtual ~mafViewVTK(); 

  mafTypeMacro(mafViewVTK, mafView);

  virtual mafView*  Copy(mafObserver *Listener);
  virtual void      Create();
  virtual void			OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum VIEW_WIDGET_ID
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

  void              SetMouse(mmdMouse *mouse);

protected:
  mafRWI        *m_Rwi; 
  mafSceneGraph *m_Sg;
  mafLightKit		*m_LightKit;
  int            m_CameraPosition;
  int						 m_CameraAttach;
  mafVME				*m_AttachedVme;
  vtkMatrix4x4	*m_AttachedVmeMatrix;

  virtual mmgGui *CreateGui();

  /** Update the camera position when vme's abs matrix change. 
  This function is called from CameraUpdate when camera is attached to the vme */
  void UpdateCameraMatrix();
};
#endif
